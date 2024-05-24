#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // 创建 socket 文件描述符
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        error("Socket creation error");
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(7890);

    // 将服务器地址转换为二进制形式
    if (inet_pton(AF_INET, "192.168.43.122", &serv_addr.sin_addr) <= 0) {
        error("Invalid address / Address not supported");
    }

    // 连接到服务器
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        error("Connection Failed");
    }

    std::cout << "Connected to the server" << std::endl;

    std::string message;
    while (true) {
        std::cout << "Enter message: ";
        std::getline(std::cin, message);
        if (message == "exit") {
            break;
        }

        // 发送数据到服务器
        send(sock, message.c_str(), message.length(), 0);

        // 接收服务器回显的数据
        ssize_t valread = read(sock, buffer, BUFFER_SIZE);
        if (valread > 0) {
            std::cout << "Received from server: " << buffer << std::endl;
        } else if (valread == 0) {
            std::cout << "Server closed the connection" << std::endl;
            break;
        } else {
            error("Read error");
        }

        std::memset(buffer, 0, BUFFER_SIZE); // 清空缓冲区
    }

    close(sock);
    return 0;
}
