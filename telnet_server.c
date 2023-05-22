#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

int kiemtra_user(char username[60], char password[60]);
void XoaPhanTu(int arr[], int &n, int index);

int main()
{
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("bind() failed");
        return 1;
    }

    if (listen(listener, 5))
    {
        perror("listen() failed");
        return 1;
    }

    fd_set fdread, fdtest;
    FD_ZERO(&fdread);
    FD_SET(listener, &fdread);

    char buf[256];

    int users[64];     // Mang socket client da dang nhap
    int num_users = 0; // So client da dang nhap

    while (1)
    {
        fdtest = fdread;

        int ret = select(FD_SETSIZE, &fdtest, NULL, NULL, NULL);

        if (ret < 0)
        {
            perror("select() failed");
            break;
        }

        for (int i = 0; i < FD_SETSIZE; i++)
        {
            if (FD_ISSET(i, &fdtest))
            {
                if (i == listener)
                {
                    // Chap nhan ket noi
                    int client = accept(listener, NULL, NULL);
                    char msg[] = "gửi user và pass\n";
                    send(client, msg, strlen(msg), 0);
                    if (client < FD_SETSIZE)
                    {
                        printf("New client connected: %d\n", client);
                        FD_SET(client, &fdread);
                    }
                    else
                    {
                        printf("Too many connections.\n");
                        close(client);
                    }
                }
                else
                {
                    // Nhan du lieu
                    int ret = recv(i, buf, sizeof(buf), 0);
                    if (ret <= 0)
                    {
                        printf("Client %d disconnected.\n", i);
                        close(i);
                        FD_CLR(i, &fdread);

                        int index;
                        for (int k = 0; k < num_users; k++)
                        {
                            if (users[k] == i)
                            {
                                index = k;
                                break;
                            }
                        }
                        XoaPhanTu(users, num_users, index);
                    }
                    else
                    {
                        buf[ret] = 0;
                        printf("Received from %d: %s\n", i, buf);

                        // Kiem tra trang thai dang nhap cua client
                        int client = i;

                        int j = 0;
                        for (; j < num_users; j++)
                            if (users[j] == client)
                                break;

                        if (j == num_users)
                        {
                            // Chua dang nhap
                            char username[60], password[60];
                            sscanf(buf, "%s %s", username, password);
                            if (kiemtra_user(username, password))
                            {
                                char msg[] = "đúng mật khẩu\n";
                                send(i, msg, strlen(msg), 0);
                                users[num_users] = client;
                                num_users++;
                            }
                            else
                            {
                                char msg[] = "sai mật khẩu\n";
                                send(i, msg, strlen(msg), 0);
                            }
                        }
                        else
                        {
                            // da dang nhap
                            // nhan yeu cau tu client

                            buf[strcspn(buf, "\n")] = 0;
                            strcat(buf, " > out.txt");
                            system(buf);

                            char *filename = "out.txt";
                            FILE *f = fopen(filename, "r");
                            int ret;

                            char buf_back_to_client[2048];
                            while (!feof(f))
                            {
                                ret = fread(buf_back_to_client, 1, sizeof(buf_back_to_client), f);
                                if (ret <= 0)
                                    break;
                                send(client, buf_back_to_client, ret, 0);
                            }
                        }
                    }
                }
            }
        }
    }
}

int kiemtra_user(char username[60], char password[60])
{
    FILE *dg;
    char No1[60];
    char No2[60];

    if ((dg = fopen("password.txt", "r")) == NULL)
        puts("File can not open !\n"), exit(1);

    while (!feof(dg))
    {
        fscanf(dg, "%s %s", No1, No2);

        if ((strcmp(username, No1) == 0) && (strcmp(password, No2) == 0))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    fclose(dg);
    return 0;
}

void XoaPhanTu(int arr[], int &n, int index)
{
    // neu dia chi xoa nho hon 0 thi xoa phan tu dau tien
    if (index < 0)
    {
        index = 0;
    }
    // neu dia chi xoa lon hon hoac bang n thi xoa phan tu cuoi cung
    if (index >= n)
    {
        index = n - 1;
    }
    // Dich chuyen mang ve ben trai tu vi tri xoa
    for (int i = index; i < n - 1; i++)
    {
        arr[i] = arr[i + 1];
    }
    // sau khi xoa giam so luong phan tu mang
    n--;
}
