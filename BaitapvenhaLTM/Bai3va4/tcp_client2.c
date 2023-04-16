#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 256
struct sinhvien
{
    char MSSV[20];
    char HoTen[50];
    int NgaySinh;
    int ThangSinh;
    int NamSinh;
    float DiemTBC;
};

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf(" %s <địa chỉ IP> <cổng>\n", argv[0]);
        exit(1);
    }

    int client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_sock == -1)
    {
        perror("socket() failed");
        exit(1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("connect() failed");
        exit(1);
    }

    while (1)
    {
        fflush(stdin);
        struct sinhvien sinhvien;
        fflush(stdin);

        printf("Nhap MSSV: ");
        fflush(stdin);
        scanf("%s", sinhvien.MSSV);

        printf("Nhap ho ten: ");
        scanf(" %[^\n]", sinhvien.HoTen);

        printf("Nhap ngay sinh: ");
        fflush(stdin);
        scanf("%d", &sinhvien.NgaySinh);

        printf("Nhap thang sinh: ");
        fflush(stdin);
        scanf("%d", &sinhvien.ThangSinh);

        printf("Nhap nam sinh: ");
        fflush(stdin);
        scanf("%d", &sinhvien.NamSinh);

        printf("Nhap diem trung binh: ");
        fflush(stdin);
        scanf("%f", &sinhvien.DiemTBC);
        char message[256];
        sprintf(message, "%s %s %d-%02d-%02d %.2f", sinhvien.MSSV, sinhvien.HoTen, sinhvien.NamSinh, sinhvien.ThangSinh, sinhvien.NgaySinh, sinhvien.DiemTBC);
        printf("%s",message);
        send(client_sock,message,strlen(message),0);
    }

    close(client_sock);
    return 0;
}
