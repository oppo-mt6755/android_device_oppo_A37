// SPDX-License-Identifier: Apache-2.0
#define LOG_TAG "NfcMt6605"

#include <android-base/file.h>
#include <android-base/unique_fd.h>
#include <android/hardware/nfc/1.0/types.h>
#include <hardware/nfc.h>
#include <log/log.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>

#include <array>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <memory>
#include <mutex>
#include <new>
#include <optional>
#include <thread>
#include <type_traits>

namespace {
using android::base::ReadFully;
using android::base::unique_fd;
using android::hardware::nfc::V1_0::NfcEvent;
using android::hardware::nfc::V1_0::NfcStatus;
using namespace std::chrono_literals;

constexpr char kSocketPath[] = "/data/vendor/nfc/mtknfc_server";
constexpr size_t kPacketCapacity = 520;
constexpr uint8_t kRfNotification = 0x61;
constexpr uint8_t kRfInterfaceActivated = 0x05;
constexpr uint8_t kRfDeactivate = 0x06;
constexpr uint8_t kIso15693Protocol = 0x83;
constexpr uint8_t kIso15693PollMode = 0x06;
constexpr uint8_t kDeactivateToIdle = 0x00;
constexpr std::array<uint8_t, 4> kDeactivateToDiscovery{0x21, kRfDeactivate, 0x01, 0x03};

// The stock middleware uses little-endian 32-bit fields on this socket.
enum class Message : uint32_t {
    Open = 1001, OpenComplete,
    Close, CloseComplete,
    Write, Read,
    CoreInitialized, CoreInitializedComplete,
    PreDiscover, PreDiscoverComplete,
    RequestControl, ControlGranted, ReleaseControl,
    PowerCycle, PowerCycleComplete,
};

struct Header {
    Message type;
    uint32_t size;
};

struct Packet {
    uint32_t length;
    std::array<uint8_t, kPacketCapacity> bytes;
};

static_assert(sizeof(Header) == 8);
static_assert(sizeof(Packet) == 524);

unique_fd Connect() {
    sockaddr_un address{};
    address.sun_family = AF_UNIX;
    static_assert(sizeof(kSocketPath) <= sizeof(address.sun_path));
    memcpy(address.sun_path, kSocketPath, sizeof(kSocketPath));

    // The stock backend is started after /data/vendor becomes available.
    for (int attempt = 0; attempt < 30; ++attempt) {
        unique_fd fd(socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0));
        if (fd < 0) break;
        const timeval timeout{.tv_sec = 1, .tv_usec = 0};
        if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) != 0) break;
        if (TEMP_FAILURE_RETRY(connect(fd, reinterpret_cast<sockaddr*>(&address),
                                       sizeof(address))) == 0) {
            return fd;
        }
        if (errno != ENOENT && errno != ECONNREFUSED) break;
        std::this_thread::sleep_for(100ms);
    }
    ALOGE("Unable to connect to the MT6605 backend");
    return {};
}

class Connection {
public:
    Connection(unique_fd fd, nfc_stack_callback_t* event, nfc_stack_data_callback_t* data)
        : fd_(std::move(fd)), event_(event), data_(data) {}

    bool Connected() const { return connected_; }

    void Stop() {
        if (connected_.exchange(false)) shutdown(fd_, SHUT_RDWR);
    }

    bool Send(Message type, const void* payload, uint32_t length) {
        const Header header{type, length};
        std::lock_guard<std::mutex> lock(write_lock_);
        if (!Connected()) return false;
        if (SendFully(&header, sizeof(header)) && SendFully(payload, length)) return true;
        Stop();
        return false;
    }

    bool Write(Packet& packet) {
        if (iso15693Active_ && packet.length == kDeactivateToDiscovery.size() &&
            memcmp(packet.bytes.data(), kDeactivateToDiscovery.data(), packet.length) == 0) {
            // MT6605 can acknowledge the direct transition without restarting polling.
            // An idle notification makes the NCI stack start discovery again if enabled.
            packet.bytes[3] = kDeactivateToIdle;
            ALOGD("Returning NFC-V to idle before resuming discovery");
        }
        return Send(Message::Write, &packet, sizeof(packet));
    }

    int PrepareDiscovery() {
        constexpr uint32_t reserved = 0;
        std::unique_lock<std::mutex> lock(discovery_lock_);
        discovery_result_.reset();
        if (!Send(Message::PreDiscover, &reserved, sizeof(reserved))) return -EIO;
        // Android 10 sends RF_DISCOVER as soon as prediscover() returns.
        // The MT6605 middleware consumes incoming replies while preparing the radio.
        if (!discovery_ready_.wait_for(lock, 2s, [this] {
                return discovery_result_.has_value() || !Connected();
            })) {
            Stop();
            return -ETIMEDOUT;
        }
        return discovery_result_ == NfcStatus::OK ? 0 : -EIO;
    }

    void ReadReplies() {
        Header header{};
        Packet packet{};
        while (ReadFully(fd_, &header, sizeof(header))) {
            if (header.size > sizeof(packet) || !ReadFully(fd_, &packet, header.size)) break;
            if (header.type == Message::Read) {
                if (header.size < sizeof(packet.length) || packet.length > packet.bytes.size() ||
                    packet.length > header.size - sizeof(packet.length)) {
                    ALOGE("Invalid NCI packet length from the MT6605 backend");
                    break;
                }
                if (packet.length != 0) {
                    UpdatePollingState(packet);
                    data_(packet.length, packet.bytes.data());
                }
                continue;
            }

            if (header.size != sizeof(uint32_t)) {
                ALOGE("Invalid event length from the MT6605 backend");
                break;
            }
            const auto status = packet.length == 0 ? NfcStatus::OK : NfcStatus::FAILED;
            switch (header.type) {
                case Message::OpenComplete:
                case Message::PowerCycleComplete:
                    iso15693Active_ = false;
                    Notify(NfcEvent::OPEN_CPLT, status);
                    break;
                case Message::CloseComplete:
                    iso15693Active_ = false;
                    Notify(NfcEvent::CLOSE_CPLT, status);
                    break;
                case Message::CoreInitializedComplete:
                    Notify(NfcEvent::POST_INIT_CPLT, status);
                    break;
                case Message::PreDiscoverComplete: {
                    std::lock_guard<std::mutex> lock(discovery_lock_);
                    discovery_result_ = status;
                    discovery_ready_.notify_one();
                    break;
                }
                case Message::RequestControl:
                    Notify(NfcEvent::REQUEST_CONTROL, status);
                    break;
                case Message::ReleaseControl:
                    Notify(NfcEvent::RELEASE_CONTROL, status);
                    break;
                default:
                    ALOGW("Ignoring unknown MT6605 message %u", static_cast<uint32_t>(header.type));
                    break;
            }
        }
        const bool disconnected = connected_.exchange(false);
        {
            std::lock_guard<std::mutex> lock(discovery_lock_);
            discovery_ready_.notify_one();
        }
        if (disconnected) {
            shutdown(fd_, SHUT_RDWR);
            ALOGE("Disconnected from the MT6605 backend");
            Notify(NfcEvent::ERROR, NfcStatus::ERR_TRANSPORT);
        }
    }

private:
    void UpdatePollingState(const Packet& packet) {
        if (packet.length < 3 || packet.bytes[0] != kRfNotification) return;
        if (packet.bytes[1] == kRfInterfaceActivated && packet.length >= 7) {
            iso15693Active_ = packet.bytes[5] == kIso15693Protocol &&
                    packet.bytes[6] == kIso15693PollMode;
        } else if (packet.bytes[1] == kRfDeactivate) {
            iso15693Active_ = false;
        }
    }

    bool SendFully(const void* payload, size_t length) {
        const auto* data = static_cast<const uint8_t*>(payload);
        while (length != 0) {
            const ssize_t count = TEMP_FAILURE_RETRY(send(fd_, data, length, MSG_NOSIGNAL));
            if (count <= 0) return false;
            data += count;
            length -= count;
        }
        return true;
    }

    void Notify(NfcEvent event, NfcStatus status) {
        event_(static_cast<nfc_event_t>(event), static_cast<nfc_status_t>(status));
    }

    const unique_fd fd_;
    nfc_stack_callback_t* const event_;
    nfc_stack_data_callback_t* const data_;
    std::mutex write_lock_;
    std::mutex discovery_lock_;
    std::condition_variable discovery_ready_;
    std::optional<NfcStatus> discovery_result_;
    std::atomic<bool> connected_{true};
    std::atomic<bool> iso15693Active_{false};
};

class Transport {
public:
    ~Transport() {
        if (connection_) connection_->Stop();
    }

    int Open(nfc_stack_callback_t* event, nfc_stack_data_callback_t* data) {
        if (!event || !data) return -EINVAL;
        std::lock_guard<std::mutex> lock(lock_);
        if (!connection_ || !connection_->Connected()) {
            auto fd = Connect();
            if (fd >= 0) {
                connection_ = std::make_shared<Connection>(std::move(fd), event, data);
                std::thread([connection = connection_] { connection->ReadReplies(); }).detach();
            }
        }
        if (SendControl(Message::Open) == 0) return 0;

        // Android waits for OPEN_CPLT even when the transport could not be opened.
        std::thread([event] {
            event(static_cast<nfc_event_t>(NfcEvent::OPEN_CPLT),
                  static_cast<nfc_status_t>(NfcStatus::ERR_TRANSPORT));
        }).detach();
        return -EIO;
    }

    int Control(Message type) {
        std::lock_guard<std::mutex> lock(lock_);
        return SendControl(type);
    }

    int PrepareDiscovery() {
        std::lock_guard<std::mutex> lock(lock_);
        return connection_ ? connection_->PrepareDiscovery() : -EIO;
    }

    int Write(uint16_t length, const uint8_t* data) {
        if (!data || length == 0 || length > kPacketCapacity) return -EINVAL;
        Packet packet{};
        packet.length = length;
        memcpy(packet.bytes.data(), data, length);
        std::lock_guard<std::mutex> lock(lock_);
        if (!connection_ || !connection_->Write(packet)) {
            return -EIO;
        }
        return length;
    }

private:
    int SendControl(Message type) {
        constexpr uint32_t reserved = 0;
        return connection_ && connection_->Send(type, &reserved, sizeof(reserved)) ? 0 : -EIO;
    }

    std::mutex lock_;
    std::shared_ptr<Connection> connection_;
};

struct LegacyDevice {
    nfc_nci_device_t device{};
    Transport transport;
};

static_assert(std::is_standard_layout_v<LegacyDevice>);

Transport& GetTransport(const nfc_nci_device_t* device) {
    return reinterpret_cast<LegacyDevice*>(const_cast<nfc_nci_device_t*>(device))->transport;
}

int OpenModule(const hw_module_t* module, const char* name, hw_device_t** out) {
    if (!name || strcmp(name, NFC_NCI_CONTROLLER) != 0 || !out) return -EINVAL;
    auto* instance = new (std::nothrow) LegacyDevice;
    if (!instance) return -ENOMEM;

    auto& device = instance->device;
    device.common.tag = HARDWARE_DEVICE_TAG;
    device.common.module = const_cast<hw_module_t*>(module);
    device.common.close = [](hw_device_t* dev) {
        delete reinterpret_cast<LegacyDevice*>(dev);
        return 0;
    };
    device.open = [](const nfc_nci_device_t* dev, nfc_stack_callback_t* event,
                     nfc_stack_data_callback_t* data) { return GetTransport(dev).Open(event, data); };
    device.write = [](const nfc_nci_device_t* dev, uint16_t length, const uint8_t* data) {
        return GetTransport(dev).Write(length, data);
    };
    device.core_initialized = [](const nfc_nci_device_t* dev, uint8_t*) {
        return GetTransport(dev).Control(Message::CoreInitialized);
    };
    device.pre_discover = [](const nfc_nci_device_t* dev) {
        return GetTransport(dev).PrepareDiscovery();
    };
    device.close = [](const nfc_nci_device_t* dev) {
        return GetTransport(dev).Control(Message::Close);
    };
    device.control_granted = [](const nfc_nci_device_t* dev) {
        return GetTransport(dev).Control(Message::ControlGranted);
    };
    device.power_cycle = [](const nfc_nci_device_t* dev) {
        return GetTransport(dev).Control(Message::PowerCycle);
    };

    *out = &device.common;
    return 0;
}

hw_module_methods_t methods = {.open = OpenModule};
}  // namespace

extern "C" {
nfc_nci_module_t HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = HARDWARE_MODULE_API_VERSION(1, 0),
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = NFC_NCI_HARDWARE_MODULE_ID,
        .name = "MediaTek MT6605 NFC transport",
        .author = "The LineageOS Project",
        .methods = &methods,
    },
};
}
