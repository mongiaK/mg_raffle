#include <deque>
#include <iostream>
#include <netinet/in.h>
#include <ostream>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "rpc.pb.h"

#define MAX_CONN_BUF 1024
#define MAGIC "peng"

enum {
    M_MAGIC = 1,
    M_HEADER = 2,
    M_RPC_HEADER = 3,
    M_RPC_CONTENT = 4,
};

struct MLengthHeader {
    int rpc_header_length;
    int rpc_body_length;
};

using namespace std;

int main(int argc, char *argv[]) {
    // if (argc != 3) {
    //     cout << "Example:./myTceClient 127.0.0.1 5005" << endl;
    //     return 1;
    // }
    //
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return -1;
    }

    // struct hostent *h;
    // if ((h = gethostbyname(argv[1])) == 0) {
    //     cout << "gethostbyname failed" << endl;
    //     close(sockfd);
    //     return -1;
    // }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8090);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    int conres =
        connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (conres != 0) {
        perror("connect failed");
        close(sockfd);
        return -1;
    }

    char buf[1024];
    M::RpcHeader rpc_header;
    rpc_header.set_type(M::RPC_TYPE_RPC_REQUEST);
    rpc_header.set_func("hello");
    M::HelloReq helloreq;
    helloreq.set_name("hello");
    MLengthHeader mlh;
    mlh.rpc_header_length = int(rpc_header.ByteSizeLong());
    mlh.rpc_body_length = int(helloreq.ByteSizeLong());

    memcpy(buf, "peng", 4);
    memcpy(buf + 4, &mlh, sizeof(MLengthHeader));
    
    rpc_header.SerializeToArray(buf + 4 + sizeof(MLengthHeader),
                                rpc_header.ByteSizeLong());
    helloreq.SerializeToArray(buf + 4 + sizeof(MLengthHeader) +
                                  rpc_header.ByteSizeLong(),
                              helloreq.ByteSizeLong());
    int length = 4 + sizeof(MLengthHeader) + rpc_header.ByteSizeLong() +
                 helloreq.ByteSizeLong();

    for (int ii = 0; ii < 1; ii++) {
        int iret;
        iret = send(sockfd, buf, length, 0);
        if (iret <= 0) {
            perror("send");
            break;
        }
        memset(buf, 0, sizeof(buf));

        iret = recv(sockfd, buf, sizeof(buf), 0);
        if (iret <= 0) {
            cout << "recv nothing" << endl;
            break;
        }
        memcpy(&mlh, buf + 4, sizeof(MLengthHeader));
        M::RpcHeader header;
        M::HelloRes res;
        header.ParseFromArray(buf + 4 + sizeof(MLengthHeader),
                              mlh.rpc_header_length);
        res.ParseFromArray(buf + 4 + sizeof(MLengthHeader) +
                               mlh.rpc_header_length,
                           mlh.rpc_body_length);
        std::cout << res.SerializeAsString() << std::endl;
    }
    close(sockfd);
    return 0;
}
