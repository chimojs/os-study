#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

DWORD WINAPI Sender(LPVOID Param)
{
    char sendData[255];
    SOCKET sclient = *(SOCKET*)Param;
    while (true)
    {
        scanf("%s", sendData);
        send(sclient, sendData, strlen(sendData), 0);
    }
    return 0;
}

DWORD WINAPI Receiver(LPVOID Param)
{
    char recData[255];
    SOCKET sclient = *(SOCKET*)Param;
    while (true)
    {
        int ret = recv(sclient, recData, 255, 0);
        if (ret > 0)
        {
            if (recData[0] == 'q')
            {
                break;
            }
            recData[ret] = 0x00;
            printf("receive from client:");
            printf("%s\n", recData);
        }
    }
    return 0;
}

int main(int argc, char* argv[])
{
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA wsaData;
    if(WSAStartup(sockVersion, &wsaData)!=0)
    {
        return -1;
    }

    SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(slisten == INVALID_SOCKET)
    {
        printf("socket error !");
        return -1;
    }
 
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(12000);
    sin.sin_addr.S_un.S_addr = INADDR_ANY; 
    if(bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
    {
        printf("bind error !");
    }

    if(listen(slisten, 1) == SOCKET_ERROR)
    {
        printf("listen error !");
         return -1;
    }
 
    SOCKET sClient;
    sockaddr_in remoteAddr;
    int nAddrlen = sizeof(remoteAddr);

    while (true)
    {
        printf("wait connecting...\n");
        sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
        if (sClient == INVALID_SOCKET)
        {
            printf("accept error !");
            continue;
        }
        printf("received a connection£º%s \r\n", inet_ntoa(remoteAddr.sin_addr));
        break;
    }

     DWORD ThreadID_Sender;
     HANDLE ThreadHandle_Sender;

     ThreadHandle_Sender = CreateThread(
         NULL,
         0,
         Sender,
         &sClient,
         0,
         &ThreadID_Sender);

    DWORD ThreadID_Receiverer;
    HANDLE ThreadHandle_Receiverer;


    ThreadHandle_Receiverer = CreateThread(
        NULL,
        0,
        Receiver,
        &sClient,
        0,
        &ThreadID_Receiverer);

    if (ThreadHandle_Sender != NULL)
    {
        WaitForSingleObject(ThreadHandle_Receiverer, INFINITE);
        CloseHandle(ThreadHandle_Sender);
        CloseHandle(ThreadHandle_Receiverer);
        closesocket(sClient);
    }

    closesocket(slisten);
    WSACleanup();
    return 0;
}
