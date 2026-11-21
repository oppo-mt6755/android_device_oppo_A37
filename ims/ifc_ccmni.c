/*
 * Copyright (C) 2026 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* MediaTek libnetutils additions used by mtk-ril, backed by the ccmni driver ioctls. */

#include <net/if.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/system_properties.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <linux/sockios.h>

#define SIOCSTXQSTATE   (SIOCDEVPRIVATE + 0)
#define SIOCCCMNICFG    (SIOCDEVPRIVATE + 1)

static int ccmni_ioctl(const char *ifname, unsigned long cmd, int value)
{
    struct ifreq ifr;
    int sock, ret;

    sock = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
    if (sock < 0)
        return -1;

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
    ifr.ifr_ifru.ifru_ivalue = value;

    ret = ioctl(sock, cmd, &ifr);
    close(sock);

    return ret;
}

int ifc_set_txq_state(const char *ifname, int state)
{
    return ccmni_ioctl(ifname, SIOCSTXQSTATE, state);
}

int ifc_ccmni_md_cfg(const char *ifname, int md_id)
{
    return ccmni_ioctl(ifname, SIOCCCMNICFG, md_id);
}

int dhcp_start(const char *ifname)
{
    (void)ifname;
    return -1;
}

int dhcp_get_results(const char *ifname, char *ipaddr, char *gateway,
        unsigned int *prefix_length, char *dns[], char *server,
        unsigned int *lease, char *vendor_info, char *domain, char *mtu)
{
    (void)ifname; (void)ipaddr; (void)gateway; (void)prefix_length;
    (void)dns; (void)server; (void)lease; (void)vendor_info; (void)domain;
    (void)mtu;
    return -1;
}

/* The old MTK netd published a /64 prefix property. Oreo learns the same
 * information through rtnetlink; derive it from a real configured global
 * address when that historical property is absent. */
int property_get(const char *key, char *value, const char *default_value)
{
    int length = __system_property_get(key, value);
    if (length == 0) {
        char ifname[IFNAMSIZ];
        int consumed = 0;
        if (sscanf(key, "net.ipv6.%15[^.].prefix%n", ifname, &consumed) == 1 &&
                consumed > 0 && key[consumed] == '\0' &&
                strncmp(ifname, "ccmni", 5) == 0) {
            struct ifaddrs *addresses = NULL;
            if (getifaddrs(&addresses) == 0) {
                for (struct ifaddrs *ifa = addresses; ifa; ifa = ifa->ifa_next) {
                    if (!ifa->ifa_addr || !ifa->ifa_netmask ||
                            strcmp(ifa->ifa_name, ifname) != 0 ||
                            ifa->ifa_addr->sa_family != AF_INET6) continue;
                    const struct in6_addr *address =
                            &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
                    const struct in6_addr *mask =
                            &((struct sockaddr_in6 *)ifa->ifa_netmask)->sin6_addr;
                    static const unsigned char mask64[16] =
                            {255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0};
                    if (IN6_IS_ADDR_LINKLOCAL(address) || IN6_IS_ADDR_UNSPECIFIED(address) ||
                            IN6_IS_ADDR_LOOPBACK(address) || IN6_IS_ADDR_MULTICAST(address) ||
                            memcmp(mask->s6_addr, mask64, sizeof(mask64)) != 0) continue;
                    struct in6_addr prefix = *address;
                    memset(prefix.s6_addr + 8, 0, 8);
                    if (inet_ntop(AF_INET6, &prefix, value, PROP_VALUE_MAX)) {
                        length = strlen(value);
                        break;
                    }
                }
                freeifaddrs(addresses);
            }
        }
    }
    if (length == 0 && default_value) {
        length = strnlen(default_value, PROP_VALUE_MAX - 1);
        memcpy(value, default_value, length);
        value[length] = '\0';
    }
    return length;
}
