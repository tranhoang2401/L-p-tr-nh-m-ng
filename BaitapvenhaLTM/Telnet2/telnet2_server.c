#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/wait.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int check_credentials(char *username, char *password)
{
    FILE *fp;
    char line[BUFFER_SIZE];
    char *token;
    fp = fopen("credentials.txt", "r");
    if (fp == NULL)
    {
        perror("Error opening credentials file");
        return 0;
    }

    while (fgets(line, sizeof(line), fp))
    {

        token = strtok(line, " ");
        if (token != NULL && strcmp(token, username) == 0)
        {
            token = strtok(NULL, " \n");
            if (token != NULL && strcmp(token, password) == 0)
            {
                fclose(fp);
                return 1; 
            }
        }
    }

    fclose(fp);
    return 0; 
}

int main()
{
    int server_fd, client_fds[MAX_CLIENTS], max_clients = MAX_CLIENTS;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addrlen;
    char buffer[BUFFER_SIZE];

    for (int i = 0; i < max_clients; i++)
    {
        client_fds[i] = 0;
    }

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8888);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    addrlen = sizeof(client_addr);

    while (1)
    {
        int new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
        if (new_socket < 0)
        {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("New connection, socket fd is %d, IP is : %s, port : %d\n",
               new_socket, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        pid_t pid = fork();
        if (pid == 0)
        {
            close(server_fd);
            char *welcome_message = "Welcome to the Telnet server!\nPlease enter your username and password!\n";
            send(new_socket, welcome_message, strlen(welcome_message), 0);

            while (1)
            {
                memset(buffer, 0, BUFFER_SIZE);
                int valread = recv(new_socket, buffer, BUFFER_SIZE, 0);
                // Xá»­ lÃ½ dá»¯ liá»‡u tá»« client
                if (valread <= 0)
                {
                    struct sockaddr_in addr;
                    socklen_t addr_len = sizeof(addr);
                    getpeername(new_socket, (struct sockaddr *)&addr, &addr_len);
                    printf("Host disconnected, ip %s, port %d\n",
                           inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
                    close(new_socket);
                    exit(EXIT_SUCCESS);
                }
                else
                {
                    if (strncmp(buffer, "userpass:", 9) == 0)
                    {
                        char *username = strtok(buffer + 9, " ");
                        char *password = strtok(NULL, "\n");
                        int authenticated = check_credentials(username, password);

                        if (authenticated)
                        {
                            char *success_message = "Login successful\n";
                            send(new_socket, success_message, strlen(success_message), 0);
                        }
                        else
                        {
                            char *error_message = "Invalid username or password\n";
                            send(new_socket, error_message, strlen(error_message), 0);
                        }
                    }
                    else
                    {
                        char command[BUFFER_SIZE];
                        snprintf(command, BUFFER_SIZE, "%.900s > out.txt", buffer);
                        system(command);

                        FILE *fp = fopen("out.txt", "r");

                        if (fp != NULL)
                        {
                            char result_buffer[BUFFER_SIZE];
                            memset(result_buffer, 0, BUFFER_SIZE);
                            fread(result_buffer, sizeof(char), BUFFER_SIZE - 1, fp);
                            send(new_socket, result_buffer, strlen(result_buffer), 0);
                            fseek(fp, 0, SEEK_END);
                            long size = ftell(fp);
                            if (size == 0)
                            {
                                char *error_message = "Error executing command\n";
                                send(new_socket, error_message, strlen(error_message) + 1, 0);
                            }
                            fclose(fp);
                        }
                    }
                }
            }
        }
        else if (pid > 0)
        {
            close(new_socket);
        }
        else
        {
            perror("Fork failed");
        }
    }

    return 0;
}