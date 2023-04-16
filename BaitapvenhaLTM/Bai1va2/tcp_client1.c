#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 256

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("%s <địa chỉ IP> <cổng>\n", argv[0]);
        exit(1);
    }

    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client == -1)
    {
        perror("socket() failed");
        exit(1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));


    if (connect(client, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("connect() failed");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    
    int received = recv(client, buffer, BUFFER_SIZE - 1, 0);
    if (received == -1)
    {
        perror("recv() failed");
        exit(1);
    }
    buffer[received] = 0;
    printf("Phản hồi server: %s\n", buffer);

    while (1)
    {
        printf(":");
        fgets(buffer, sizeof(buffer), stdin);

        send(client, buffer, strlen(buffer), 0);

        if (strncmp(buffer, "exit", 4) == 0)
            break;
    }

    close(client);


}