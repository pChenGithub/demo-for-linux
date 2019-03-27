/*************************************************************************
	> File Name: test.c
	> Author: chen.peng
	> Mail: 404285202@qq.com 
	> Created Time: 2019年03月20日 星期三 18时29分01秒
 ************************************************************************/

#include<stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main(int argc, char *argv[]) {


	struct hostent *h;

	if((h=gethostbyname(argv[1]))==NULL)
	{
		fprintf(stderr,"不能得到IP\n");
		return -1;
	}
	printf("HostName :%s\n",h->h_name);
	printf("IP Address :%s\n",inet_ntoa(*((struct in_addr *)h->h_addr_list[0])));


	return 0;

}
