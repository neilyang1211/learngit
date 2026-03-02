#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include "server.h"

int initialize_server()
{
	
	/*int socket(int domain, int type, int protocol)
	* 1.地址 AF_INET: IPv4   AF_INET6: IPv6    AF_UNIX: 本地通信
	* 2.套接字类型  SOCK_STREAM: 面向连接的 TCP     SOCK_DGRAM: 无连接的 UDP
	* 3.协议 0: 默认协议（通常 TCP 对应 IPPROTO_TCP，UDP 对应 IPPROTO_UDP）
	*/
	int sockfd;//创建套接字的描述符，监听客户端连接的套接字
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0)
	{
        	perror("socket failed");
        	return -1;
   	}
   	else
   	{
		printf("socket successfully\n");
   	}

	struct sockaddr_in server_addr;//存储服务端地址的结构体，ip和端口
	memset(&server_addr,0,sizeof(server_addr)); /*清空结构体的内存，方便写入数据初始化*/

 	/*struct sockaddr_in {
    	* sa_family_t sin_family;   // 地址族，通常是 AF_INET,即ipv4
    	* in_port_t sin_port;       // 端口号，网络字节序
    	* struct in_addr sin_addr;  // IP 地址（结构体，存储地址值）
    	* unsigned char sin_zero[8]; // 填充字段，必须置为 0,不用手动设置
    	*/
   	server_addr.sin_family = AF_INET;
   	server_addr.sin_addr.s_addr = INADDR_ANY;
   	server_addr.sin_port = htons(774);

	/*等待连接是bind，用于连接是connect，流程一样，函数不一样
    	* int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
    	* 1.sockfd：套接字描述符，通常由 socket() 函数返回。
    	* 2.addr：指向 struct sockaddr 类型的指针，这个结构体包含了要绑定的 IP 地址和端口号信息。
    	* 常见的地址结构是 struct sockaddr_in，用于IPv4。
    	* 3.addrlen：addr 结构的大小，通常传入 sizeof(struct sockaddr_in)。
    	* 成功：返回 0。失败：返回 -1，你可以通过 perror() 或 strerror() 获取错误信息。
    	*/
   	if(bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0)
   	{
        	perror("bind failed!\n");
        	close(sockfd);
		return -1;
   	}
   	else
   	{
		printf("bind successfully\n");
   	}

  	/*int listen(int sockfd, int backlog)
    	* sockfd：已绑定（bind()）的套接字描述符，通常是通过 socket() 创建并使用 bind() 绑定到地址和端口的套接字
    	* backlog：指定连接请求队列的大小。它表示操作系统允许在没有调用 accept() 的情况下，等待的最大客户端连接数。
    	* 成功：返回 0。失败：返回 -1
    	*/
	if(listen(sockfd,10) < 0)
	{
       		perror("listen failed\n");
       		return -1;
   	}
   	else
   	{
		printf("listen successfully\n");
   	}

   	return sockfd;
}

