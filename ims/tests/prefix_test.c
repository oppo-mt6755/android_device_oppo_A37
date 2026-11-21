#include <arpa/inet.h>
#include <assert.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

int property_get(const char *, char *, const char *);
static const char *address = "2408:852c:1234:5678::abcd";
static const char *mask = "ffff:ffff:ffff:ffff::";
static const char *name = "ccmni4";
static int list_error;
static struct ifaddrs entry;
static struct sockaddr_in6 ip, netmask;
int __system_property_get(const char *key, char *value) {
    const char *result = strcmp(key, "ro.test.value") == 0 ? "actual" : "";
    strcpy(value, result); return strlen(result);
}
int getifaddrs(struct ifaddrs **result) {
    if (list_error) return -1;
    memset(&entry, 0, sizeof(entry));
    ip.sin6_family = netmask.sin6_family = AF_INET6;
    assert(inet_pton(AF_INET6, address, &ip.sin6_addr) == 1);
    assert(inet_pton(AF_INET6, mask, &netmask.sin6_addr) == 1);
    entry.ifa_name = (char *)name;
    entry.ifa_addr = (struct sockaddr *)&ip;
    entry.ifa_netmask = (struct sockaddr *)&netmask;
    *result = &entry; return 0;
}
void freeifaddrs(struct ifaddrs *list) { assert(list == &entry); }
static void expect(const char *key, const char *fallback, const char *expected) {
    char value[92]; memset(value, 0x5a, sizeof(value));
    assert(property_get(key, value, fallback) == (int)strlen(expected));
    assert(strcmp(value, expected) == 0);
}
int main(void) {
    expect("ro.test.value", "fallback", "actual");
    expect("net.ipv6.ccmni4.prefix", NULL, "2408:852c:1234:5678::");
    expect("net.ipv6.ccmni4.prefix.bad", "fallback", "fallback");
    expect("net.ipv6.ccmni4.bad", NULL, "");
    expect("net.ipv6.wlan0.prefix", NULL, "");
    name = "ccmni0"; expect("net.ipv6.ccmni4.prefix", NULL, ""); name = "ccmni4";
    address = "fe80::1234"; expect("net.ipv6.ccmni4.prefix", NULL, "");
    address = "::"; expect("net.ipv6.ccmni4.prefix", NULL, "");
    address = "::1"; expect("net.ipv6.ccmni4.prefix", NULL, "");
    address = "ff02::1"; expect("net.ipv6.ccmni4.prefix", NULL, "");
    address = "2408:852c:1234:5678::abcd";
    mask = "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff";
    expect("net.ipv6.ccmni4.prefix", "fallback", "fallback");
    list_error = 1; expect("net.ipv6.ccmni4.prefix", NULL, "");
    puts("PASS: IPv6 prefix lookup, property fallback and invalid-address cases");
    return 0;
}
