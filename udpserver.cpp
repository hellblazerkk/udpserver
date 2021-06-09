// Пример простого UDP-эхо сервера
#include <stdio.h>
#include <winsock2.h>

#define PORT 666    // порт сервера
#define sHELLO "Hello, %s [%s] Sailor\n"

int main(int argc, char* argv[])
{
    char buff[1024];

    printf("UDP DEMO echo-Server\n");

    // шаг 1 - подключение библиотеки 
    if (WSAStartup(0x202, (WSADATA*)&buff[0]))
    {
        printf("WSAStartup error: %d\n",
            WSAGetLastError());
        return -1;
    }

    // шаг 2 - создание сокета
    SOCKET my_sock;
    my_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (my_sock == INVALID_SOCKET)
    {
        printf("Socket() error: %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }

    // шаг 3 - связывание сокета с локальным адресом 
    sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(PORT);

    if (bind(my_sock, (sockaddr*)&local_addr,
        sizeof(local_addr)))
    {
        printf("bind error: %d\n", WSAGetLastError());
        closesocket(my_sock);
        WSACleanup();
        return -1;
    }

    // шаг 4 обработка пакетов, присланных клиентами
    while (1)
    {
        sockaddr_in client_addr;
        int client_addr_size = sizeof(client_addr);
        int bsize = recvfrom(my_sock, &buff[0],
            sizeof(buff) - 1, 0,
            (sockaddr*)&client_addr, &client_addr_size);
        if (bsize == SOCKET_ERROR)
            printf("recvfrom() error: %d\n",
                WSAGetLastError());

        // Определяем IP-адрес клиента и прочие атрибуты
        HOSTENT* hst;
        hst = gethostbyaddr((char*)
            &client_addr.sin_addr, 4, AF_INET);
        printf("+%s [%s:%d] new DATAGRAM!\n",
            (hst) ? hst->h_name : "Unknown host",
            inet_ntoa(client_addr.sin_addr),
            ntohs(client_addr.sin_port));

        // добавление завершающего нуля
        buff[bsize] = 0;

        // Вывод на экран 
        printf("C=>S:%s\n", &buff[0]);

        // посылка датаграммы клиенту
        sendto(my_sock, &buff[0], bsize, 0,
            (sockaddr*)&client_addr, sizeof(client_addr));
    }
    return 0;
}