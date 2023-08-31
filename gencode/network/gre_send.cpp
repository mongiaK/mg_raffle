#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    // 创建原始套接字
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_GRE);
    if (sockfd < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    // 构造GRE报文
    unsigned char packet[42]; // GRE报文的长度可以根据需求进行调整
    memset(packet, 0, sizeof(packet));

    // GRE头部
    struct gre_hdr *gre = (struct gre_hdr *)packet;
    // 设置C位为1表示GRE头部包含校验和字段
    gre->flags = htons(0x8000);
    // 设置协议类型为IPv4
    gre->protocol = htons(ETH_P_IP);

    // 封装的IPv4数据包
    struct ip *iph = (struct ip *)(packet + sizeof(struct gre_hdr));
    // 设置IPv4首部字段，如源IP地址、目标IP地址、协议类型等

    // 发送GRE报文
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(struct sockaddr_in));
    sa.sin_family = AF_INET;
    // 设置目标IP地址
    inet_pton(AF_INET, "192.168.0.1", &(sa.sin_addr));
    int bytes_sent = sendto(sockfd, packet, sizeof(packet), 0,
                            (struct sockaddr *)&sa, sizeof(struct sockaddr_in));
    if (bytes_sent < 0) {
        std::cerr << "Failed to send GRE packet" << std::endl;
        close(sockfd);
        return 1;
    }

    std::cout << "GRE packet sent successfully" << std::endl;

    close(sockfd);
    return 0;
}
