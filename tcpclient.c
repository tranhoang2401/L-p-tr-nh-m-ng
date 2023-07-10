#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 256

int main(int argc, char *argv[])
{
    // Kiem tra tham so vao
    if (argc != 3)
    {
        printf("Sá»­ dá»¥ng: %s <Ä‘á»‹a chá»‰ IP> <cá»•ng>\n", argv[0]);
        exit(1);
    }

    // Tao Socket
    int client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_sock == -1)
    {
        perror("socket() failed");
        exit(1);
    }

    // Thiáº¿t láº­p Ä‘á»‹a chá»‰ vÃ  cá»•ng cá»§a server
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    // Ket noi vs server
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("connect() failed");
        exit(1);
    }

    // Nháº­p dá»¯ liá»‡u tá»« bÃ n phÃ­m vÃ  gá»­i Ä‘áº¿n server
    char buffer[BUFFER_SIZE];
    // Nháº­n pháº£n há»“i tá»« server
    int received = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
    if (received == -1)
    {
        perror("recv() failed");
        exit(1);
    }
    buffer[received] = 0;
    printf("Pháº£n há»“i tá»« server: %s\n", buffer);
    while (1)
    {
        printf("Nháº­p dá»¯ liá»‡u Ä‘á»ƒ gá»­i Ä‘áº¿n server: ");
        fflush(stdout);
        fflush(stdin);
        fgets(buffer, BUFFER_SIZE, stdin);

        // Gán giá trị liên tục cho server
        int len = strlen(buffer);
        int sent = send(client_sock, buffer, len, 0);
        if (sent != len)
        {
            perror("send() failed");
            exit(1);
        }
    }

    // ÄÃ³ng káº¿t ná»‘i
    close(client_sock);
    return 0;
}