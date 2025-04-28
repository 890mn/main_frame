#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>

void check_network_interfaces() {
    struct ifaddrs *ifaddr, *ifa;
    char host[NI_MAXHOST];

    printf("=== Conse扫网神器 - 网络接口检测 ===\n");

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) continue;

        int family = ifa->ifa_addr->sa_family;
        if (family == AF_INET) {
            // IPv4地址
            if (getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                            host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST) == 0) {
                printf("接口: %s\n", ifa->ifa_name);
                printf("  IP地址: %s\n", host);

                if (ifa->ifa_flags & IFF_MULTICAST) {
                    printf("  支持MULTICAST ✅\n");
                } else {
                    printf("  不支持MULTICAST ❌\n");
                }

                if (ifa->ifa_flags & IFF_UP) {
                    printf("  接口状态: UP\n");
                } else {
                    printf("  接口状态: DOWN\n");
                }
                printf("\n");
            }
        }
    }

    freeifaddrs(ifaddr);
}

void check_udp_multicast() {
    printf("=== Conse扫网神器 - UDP Multicast检测 ===\n");

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("239.255.0.1"); // 测试地址
    addr.sin_port = htons(7400); // DDS常用端口之一

    int ret = sendto(sock, "test", 4, 0, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        perror("Multicast发送失败");
        printf("  ➔ Multicast貌似不可用 ❌\n");
    } else {
        printf("  ➔ 成功发出Multicast UDP包 ✅\n");
    }

    close(sock);
}

int main() {
    printf("\n====== Conse超级扫网神器 ======\n\n");

    check_network_interfaces();
    check_udp_multicast();

    printf("\n====== Conse扫网结束！======\n");
    return 0;
}

