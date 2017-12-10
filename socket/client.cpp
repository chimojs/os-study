#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

DWORD WINAPI Sender(LPVOID Param)
{
    char sendData[255];
    SOCKET sclient = *(SOCKET*)Param;
    while (true)
    {
        scanf("%s", sendData);
        send(sclient, sendData, strlen(sendData), 0);
        if (sendData[0] == 'q')
            break;
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
            recData[ret] = 0x00;
            printf("receive from server:");
            printf("%s\n", recData);
        }
    }
    return 0;
}

 int main(int argc, char* argv[])
 {
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA data;
    if (WSAStartup(sockVersion, &data) != 0)
    {
        return -1;
    }
    
    SOCKET sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sclient == INVALID_SOCKET)
    {
        printf("invalid socket !");
        return -1;
     }
    
    sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(12000);
    serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
    {
        printf("connect error !");
        closesocket(sclient);
        return -1;
     }

    DWORD ThreadID_Sender;
    HANDLE ThreadHandle_Sender;

    ThreadHandle_Sender = CreateThread(
        NULL,
        0,
        Sender,
        &sclient,
        0,
        &ThreadID_Sender);

    DWORD ThreadID_Receiverer;
    HANDLE ThreadHandle_Receiverer;


    ThreadHandle_Receiverer = CreateThread(
        NULL,
        0,
        Receiver,
        &sclient,
        0,
        &ThreadID_Receiverer);

    if (ThreadHandle_Sender != NULL)
    {
        WaitForSingleObject(ThreadHandle_Sender, INFINITE);
        CloseHandle(ThreadHandle_Sender);
        CloseHandle(ThreadHandle_Receiverer);
    }
    closesocket(sclient);
    WSACleanup();
    return 0;
}
