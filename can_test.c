#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/time.h>

#define SFILE "/NS.TP.CO1T01.txt"

int main(int argc, char *argv[] )
{
	FILE *sfd;
	int s, nbytes, num=0, n, filesize, i=0;
	struct ifreq ifr;
	struct sockaddr_can addr;
	struct can_frame frame;
	int dlc = 8;
	
	struct timeval tv1;
	struct timeval tv2;
	long tv_sec=0;
	long tv_usec=0; 
	
	
	sfd = fopen(SFILE, "rb");
	if(sfd == NULL)
	{
		perror("open source file fail\n");
		return -1;
	}
	
	fseek(sfd, 0L, SEEK_END);  
    filesize = ftell(sfd);
	printf("source file size %d\n", filesize);
	rewind(sfd);
	
	
	s = socket(PF_CAN, SOCK_RAW, CAN_RAW);//创建套接字
	strcpy(ifr.ifr_name, "can0" );
	ioctl(s, SIOCGIFINDEX, &ifr); //指定 can0 设备
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	bind(s, (struct sockaddr *)&addr, sizeof(addr));//将套接字与 can0 绑定
	
	memset(&frame,0x0,sizeof(frame));
	
	frame.can_id = 0x123;//如果为扩展帧，那么 frame.can_id = CAN_EFF_FLAG | 0x123;
	 //数据长度为 1
	
	printf("start send \n");
	gettimeofday(&tv1,NULL);
	while(n = fread(frame.data, 1, dlc, sfd) > 0)
	{
		//printf("read n=%d\n", n);
		frame.can_id = 0x123;
		frame.can_dlc = dlc;
		//nbytes= write(s, &frame, sizeof(frame)); //发送数据
		nbytes = send(s, &frame, sizeof(frame), 0);
		if(nbytes != sizeof(frame)) //如果 nbytes 不等于帧长度，就说明发送失败
			printf("Error\n!");
		//else
			//printf("send ok\n");
		
		//for(i=0;i<100000;i++);
		
		//for(i=0;i<10000;i++);
		
		memset(&frame,0x0,sizeof(frame));
		
	}
	
	gettimeofday(&tv2,NULL);
	
	tv_sec = tv2.tv_sec - tv1.tv_sec;
	tv_usec = tv2.tv_usec - tv1.tv_usec;
	
	printf("time %ld us\n", tv_sec*1000000+tv_usec);
	
	printf("over close \n");
	
	fclose(sfd);
	close(s);
	

	return 0;
}

