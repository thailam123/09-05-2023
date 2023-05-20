#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

void XoaPhanTu(int arr[], int &n, int index);
void XoaPhanTu_char(char *arr[], int &n, int index);
void append(char *s, char c);

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

   int users[64];      // Mang socket client da dang nhap
   char *user_ids[64]; // Mang luu tru id cua client da dang nhap
   int num_users = 0;  // So client da dang nhap

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
         if (FD_ISSET(i, &fdtest))
         {
            if (i == listener)
            {
               // Chap nhan ket noi
               int client = accept(listener, NULL, NULL);
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
                  num_users++;

                  XoaPhanTu_char(user_ids, num_users, index);
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
                     // Xu ly cu phap yeu cau dang nhap
                     char cmd[32], id[32], tmp[32];
                     ret = sscanf(buf, "%s%s%s", cmd, id, tmp);
                     if (ret == 2)
                     {
                        if (strcmp(cmd, "client_id:") == 0)
                        {
                           char msg[] = "Dung cu phap. Gui tin nhan.\n";
                           send(client, msg, strlen(msg), 0);

                           int k = 0;
                           for (; k < num_users; k++)
                              if (strcmp(user_ids[k], id) == 0)
                                 break;

                           if (k < num_users)
                           {
                              char msg[] = "ID da ton tai. Yeu cau nhap lai.\n";
                              send(client, msg, strlen(msg), 0);
                           }
                           else
                           {
                              users[num_users] = client;
                              user_ids[num_users] = (char *)malloc(strlen(id) + 1);
                              strcpy(user_ids[num_users], id);
                              num_users++;
                           }
                        }
                        else
                        {
                           char msg[] = "Nhap sai. Yeu cau nhap lai.\n";
                           send(client, msg, strlen(msg), 0);
                        }
                     }

                     else
                     {
                        char msg[] = "Nhap sai. Yeu cau nhap lai.\n";
                        send(client, msg, strlen(msg), 0);
                     }
                  }
                  else
                  {

                     // Da dang nhap

                     char sendbuf[512];

                     // chat private
                     if (strstr(buf, "-") != NULL)
                     {
                        // lay message
                        char mes[256] = "";

                        for (int i = 0; i < strlen(buf); i++)
                        {
                           if (buf[i] == '-')
                           {
                              break;
                           }
                           append(mes, buf[i]);
                        }

                        // lay user_id nhan message
                        char *user_id;
                        char *user_id1;
                        int users_duoc_chon;
                        // user_id="- test2\n"
                        user_id = strchr(buf, '-');
                        user_id1 = user_id + 2;
                        user_id1[strcspn(user_id1, "\n")] = 0;
                        for (int i = 0; i < num_users; i++)
                        {
                           if (strcmp(user_id1, user_ids[i]) == 0)
                           {
                              users_duoc_chon = users[i];
                              break;
                           }
                        }
                        sprintf(sendbuf, "%s: %s", user_ids[j], mes);
                        send(users_duoc_chon, sendbuf, strlen(sendbuf), 0);
                     }
                     else
                     {
                        // chat public
                        sprintf(sendbuf, "%s: %s", user_ids[j], buf);
                        for (int k = 0; k < num_users; k++)
                           if (users[k] != client)
                              send(users[k], sendbuf, strlen(sendbuf), 0);
                     }
                  }
               }
            }
         }
   }

   close(listener);

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

void XoaPhanTu_char(char *arr[], int &n, int index)
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

void append(char *s, char c)
{
   int len = strlen(s);
   s[len] = c;
   s[len + 1] = '\0';
}
