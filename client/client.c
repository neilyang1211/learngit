#include <stdio.h>
#include <windows.h>
#include "screenshot.h"

int main()
{
    char bufferdata[1024] = {0};//接收服务器发来的信息数组

    SOCKET ClientSocket = remote();//获取套接字
    int bytes_received = recv(ClientSocket,bufferdata,sizeof(bufferdata),0);//接收服务端发来消息
    if(bytes_received > 0)
    {
        bufferdata[bytes_received] = '\0';
        printf("server send info is :%s",bufferdata);
    }
    else if(bytes_received == 0)
    {
        printf("connection closed by server\n");
    }
    else
    {
        perror("recv failed");
    }

    memset(bufferdata,0,1024);//接受完数据了，清空数组
    int bmpsize;//定义图片大小的变量
    BYTE *buffer;//存储图片数据的变量
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenheight = GetSystemMetrics(SM_CYSCREEN);
    
    bytes_received = recv(ClientSocket,bufferdata,sizeof(bufferdata)-1,0);//接收服务器指令
    bufferdata[bytes_received] = '\0';
    if(strcmp(bufferdata,"screenshot") == 0)
    {
        buffer = screenshot(&bmpsize);//用指针获取数据大小，buffer是保存的图片数据
        printf("start save bmp!\n");

        //开始执行保存和发送的函数
        savepng(buffer,screenWidth,screenheight,ClientSocket);
        write_data_to_socket(ClientSocket,buffer,bmpsize);
    }

    return 0;
}

