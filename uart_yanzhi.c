/****************************************************************************
 *                                                                          *
 * Copyright (c) 2014 Nuvoton Technology Corp. All rights reserved.         *
 *                                                                          *
 ****************************************************************************/
 
/****************************************************************************
 * 
 * FILENAME
 *     uart_test.c
 *
 * VERSION
 *     1.0
 *
 * DESCRIPTION
 *     This is the test program used to test the UARTs on NUC970 EV board
 *
 * DATA STRUCTURES
 *     None
 *
 * FUNCTIONS
 *     None
 *
 * HISTORY
 *     
 *
 * REMARK
 *     None
 ****************************************************************************/
#include     <stdio.h>
#include     <stdlib.h>
#include     <unistd.h> 
#include     <sys/types.h> 
#include     <sys/stat.h> 
#include     <fcntl.h> 
#include     <termios.h>  
#include     <errno.h>
#include     <string.h>
#include 	<signal.h>
#include    <pthread.h>

#define FALSE 0
#define TRUE  1

int fd[2];

pthread_t threads[10];

char buff[101];

static struct termios newtios,oldtios; /*termianal settings */
static int saved_portfd=-1;            /*serial port fd */


static void reset_tty_atexit(void)
{
	if(saved_portfd != -1)
	{
		tcsetattr(saved_portfd,TCSANOW,&oldtios);
	} 
}

/*cheanup signal handler */
static void reset_tty_handler(int signal)
{
	if(saved_portfd != -1)
	{
		tcsetattr(saved_portfd,TCSANOW,&oldtios);
	}
	_exit(EXIT_FAILURE);
}

static int open_port(const char *portname)
{
	struct sigaction sa;
	int portfd;

	printf("opening serial port:%s\n",portname);
	/*open serial port */
	if((portfd=open(portname,O_RDWR | O_NOCTTY | O_NDELAY)) < 0 )
	{
   		printf("open serial port %s fail \n ",portname);
   		return portfd;
	}

	/*get serial port parnms,save away */
	tcgetattr(portfd,&newtios);
	memcpy(&oldtios,&newtios,sizeof newtios);
	/* configure new values */
	cfmakeraw(&newtios); /*see man page */
	newtios.c_iflag |=IGNPAR; /*ignore parity on input */
	newtios.c_oflag &= ~(OPOST | ONLCR | OLCUC | OCRNL | ONOCR | ONLRET | OFILL); 
	newtios.c_cflag = CS8 | CLOCAL | CREAD;
	newtios.c_cc[VMIN]=1; /* block until 1 char received */
	newtios.c_cc[VTIME]=0; /*no inter-character timer */
	/* 115200 bps */
	cfsetospeed(&newtios,B9600);
	cfsetispeed(&newtios,B9600);
	/* register cleanup stuff */
	atexit(reset_tty_atexit);
	memset(&sa,0,sizeof sa);
	sa.sa_handler = reset_tty_handler;
	sigaction(SIGHUP,&sa,NULL);
	sigaction(SIGINT,&sa,NULL);
	sigaction(SIGPIPE,&sa,NULL);
	sigaction(SIGTERM,&sa,NULL);
	/*apply modified termios */
	saved_portfd=portfd;
	tcflush(portfd,TCIFLUSH);
	tcsetattr(portfd,TCSADRAIN,&newtios);
	return portfd;
}

void * process1(void* arg)
{
	int portfd = (int) arg;
	unsigned char i, j;
	int rev1, rev2;
	char RxBuffer[101];	

	rev1 =0;
	rev2 =0;

	while(rev2 < 100)
   	{
		rev1 = write(portfd,(buff+rev2),100);
		rev2 += rev1;
   	}

	printf("\n uart1 send %d byts\n", rev2);

	rev1 = 0;
	rev2 = 0;

	while(rev2 < 100)
	{
		rev1 = read(portfd,(RxBuffer+rev2),100);
		rev2 += rev1;
	}
		
	printf("\n uart1 receive %d bytes\n", rev2);

    for(i = 0; i < 100; i++)
        printf("%c ", RxBuffer[i]);

	for(i = 0; i < 100; i++)
	{
		if(i != RxBuffer[i])
		{
			printf("\n uart1 compare Error!!");
						
			while(1);
		}
	}

	printf("\n uart1 compare correct!!\n");
	printf("\n uart1 test done!!\n");

}	

void * process2(void* arg)
{
	int portfd = (int) arg;
	unsigned char i, j;
	int rev1, rev2;
	char RxBuffer[101];

	rev1 =0;
	rev2 =0;

	while(rev2 < 100)
   	{
		rev1 = write(portfd,(buff+rev2),100);
		rev2 += rev1;
   	}

	printf("\n uart2 send %d bytes \n", rev2);

	rev1 = 0;
	rev2 = 0;

	while(rev2 < 100)
	{
		rev1 = read(portfd,(RxBuffer+rev2),100);
		rev2 += rev1;
	}
		
	printf("\n uart2 receive %d bytes \n", rev2);

	for(i = 0; i < 100; i++)
	{
		if(i != RxBuffer[i])
		{
			printf("\n uart2 compare Error!!");
			while(1);
		}
	}

	printf("\n uart2 compare correct!!\n");
	printf("\n uart2 test done!!\n");

}
void * process3(void* arg)
{
	int portfd = (int) arg;
	unsigned char i, j;
	int rev1, rev2;
	char RxBuffer[101];	

	rev1 =0;
	rev2 =0;

	while(rev2 < 100)
   	{
		rev1 = write(portfd,(buff+rev2),100);
		rev2 += rev1;
   	}

	printf("\n uart3 send %d byts\n", rev2);

	rev1 = 0;
	rev2 = 0;

	while(rev2 < 100)
	{
		rev1 = read(portfd,(RxBuffer+rev2),100);
		rev2 += rev1;
	}
		
	printf("\n uart3 receive %d bytes\n", rev2);

    for(i = 0; i < 100; i++)
        printf("%d ", RxBuffer[i]);

	for(i = 0; i < 100; i++)
	{
		if(i != RxBuffer[i])
		{
			printf("\n uart3 compare Error!!");
						
			while(1);
		}
	}

	printf("\n uart3 compare correct!!\n");
	printf("\n uart3 test done!!\n");

}
void * process4(void* arg)
{
	int portfd = (int) arg;
	unsigned char i, j;
	int rev1, rev2;
	char RxBuffer[101];	

	rev1 =0;
	rev2 =0;

	while(rev2 < 100)
   	{
		rev1 = write(portfd,(buff+rev2),100);
		rev2 += rev1;
   	}

	printf("\n uart4 send %d byts\n", rev2);

	rev1 = 0;
	rev2 = 0;

	while(rev2 < 100)
	{
		rev1 = read(portfd,(RxBuffer+rev2),100);
		rev2 += rev1;
	}
		
	printf("\n uart4 receive %d bytes\n", rev2);

    for(i = 0; i < 100; i++)
        printf("%d ", RxBuffer[i]);

	for(i = 0; i < 100; i++)
	{
		if(i != RxBuffer[i])
		{
			printf("\n uart4 compare Error!!");
						
			while(1);
		}
	}

	printf("\n uart4 compare correct!!\n");
	printf("\n uart4 test done!!\n");

}
void * process5(void* arg)
{
	int portfd = (int) arg;
	unsigned char i, j;
	int rev1, rev2;
	char RxBuffer[101];	

	rev1 =0;
	rev2 =0;

	while(rev2 < 100)
   	{
		rev1 = write(portfd,(buff+rev2),100);
		rev2 += rev1;
   	}

	printf("\n uart5 send %d byts\n", rev2);

	rev1 = 0;
	rev2 = 0;

	while(rev2 < 100)
	{
		rev1 = read(portfd,(RxBuffer+rev2),100);
		rev2 += rev1;
	}
		
	printf("\n uart5 receive %d bytes\n", rev2);

    for(i = 0; i < 100; i++)
        printf("%d ", RxBuffer[i]);

	for(i = 0; i < 100; i++)
	{
		if(i != RxBuffer[i])
		{
			printf("\n uart5 compare Error!!");
						
			while(1);
		}
	}

	printf("\n uart5 compare correct!!\n");
	printf("\n uart5 test done!!\n");

}
void * process6(void* arg)
{
	int portfd = (int) arg;
	unsigned char i, j;
	int rev1, rev2;
	char RxBuffer[101];	

	rev1 =0;
	rev2 =0;

	while(rev2 < 100)
   	{
		rev1 = write(portfd,(buff+rev2),100);
		rev2 += rev1;
   	}

	printf("\n uart6 send %d byts\n", rev2);

	rev1 = 0;
	rev2 = 0;

	while(rev2 < 100)
	{
		rev1 = read(portfd,(RxBuffer+rev2),100);
		rev2 += rev1;
	}
		
	printf("\n uart6 receive %d bytes\n", rev2);

    for(i = 0; i < 100; i++)
        printf("%d ", RxBuffer[i]);

	for(i = 0; i < 100; i++)
	{
		if(i != RxBuffer[i])
		{
			printf("\n uart6 compare Error!!");
						
			while(1);
		}
	}

	printf("\n uart6 compare correct!!\n");
	printf("\n uart6 test done!!\n");

}
//void * process7(void* arg)
void * process7(int arg)
{
//	int portfd = (int) arg;
	int portfd =  arg;
	unsigned char i, j;
	int rev1, rev2;
	char RxBuffer[101];	
printf("uart7 open !\n");

	rev1 =0;
	rev2 =0;

	while(rev2 < 100)
   	{
		rev1 = write(portfd,(buff+rev2),100);
		rev2 += rev1;
   	}

	printf("\n uart7 send %d byts\n", rev2);

	rev1 = 0;
	rev2 = 0;

	while(rev2 < 100)
	{
		rev1 = read(portfd,(RxBuffer+rev2),100);
		rev2 += rev1;
	}
		
	printf("\n uart7 receive %d bytes\n", rev2);

    for(i = 0; i < 100; i++)
        printf("%d ", RxBuffer[i]);

	for(i = 0; i < 100; i++)
	{
		if(i != RxBuffer[i])
		{
			printf("\n uart7 compare Error!!");
						
			while(1);
		}
	}

	printf("\n uart7 compare correct!!\n");
	printf("\n uart7 test done!!\n");

}
void * process8(void* arg)
{
	int portfd = (int) arg;
	unsigned char i, j;
	int rev1, rev2;
	char RxBuffer[101];	

	rev1 =0;
	rev2 =0;

	while(rev2 < 100)
   	{
		rev1 = write(portfd,(buff+rev2),100);
		rev2 += rev1;
   	}

	printf("\n uart8 send %d byts\n", rev2);

	rev1 = 0;
	rev2 = 0;

	while(rev2 < 100)
	{
		rev1 = read(portfd,(RxBuffer+rev2),100);
		rev2 += rev1;
	}
		
	printf("\n uart8 receive %d bytes\n", rev2);

    for(i = 0; i < 100; i++)
        printf("%d ", RxBuffer[i]);

	for(i = 0; i < 100; i++)
	{
		if(i != RxBuffer[i])
		{
			printf("\n uart8 compare Error!!");
						
			while(1);
		}
	}

	printf("\n uart8 compare correct!!\n");
	printf("\n uart8 test done!!\n");

}
//void * process9(void* arg)
void * process9(int arg)
{
//	int portfd = (int) arg;
	int portfd = arg;
	unsigned char i, j;
	int rev1, rev2;
	char RxBuffer[101];	

	rev1 =0;
	rev2 =0;

	while(rev2 < 100)
   	{
		rev1 = write(portfd,(buff+rev2),100);
		rev2 += rev1;
   	}

	printf("\n uart9 send %d byts\n", rev2);

	rev1 = 0;
	rev2 = 0;

	while(rev2 < 100)
	{
		rev1 = read(portfd,(RxBuffer+rev2),100);
		rev2 += rev1;
	}
		
	printf("\n uart9 receive %d bytes\n", rev2);

    for(i = 0; i < 100; i++)
        printf("%d ", RxBuffer[i]);

	for(i = 0; i < 100; i++)
	{
		if(i != RxBuffer[i])
		{
			printf("\n uart9 compare Error!!");
						
			while(1);
		}
	}

	printf("\n uart9 compare correct!!\n");
	printf("\n uart9 test done!!\n");

}

void * process10(void* arg)
{
	int portfd = (int) arg;
	unsigned char i, j;
	int rev1, rev2;
	char RxBuffer[101];	

	rev1 =0;
	rev2 =0;

	while(rev2 < 100)
   	{
		rev1 = write(portfd,(buff+rev2),100);
		rev2 += rev1;
   	}

	printf("\n uart10 send %d byts\n", rev2);

	rev1 = 0;
	rev2 = 0;

	while(rev2 < 100)
	{
		rev1 = read(portfd,(RxBuffer+rev2),100);
		rev2 += rev1;
	}
		
	printf("\n uart10 receive %d bytes\n", rev2);

    for(i = 0; i < 100; i++)
        printf("%d ", RxBuffer[i]);

	for(i = 0; i < 100; i++)
	{
		if(i != RxBuffer[i])
		{
			printf("\n uart10 compare Error!!");
						
			while(1);
		}
	}

	printf("\n uart10 compare correct!!\n");
	printf("\n uart10 test done!!\n");

}
/*单个测试串口，适用于同时连接多个串口，但是只打开一个设备节点*/
/**
*@breif 	main()
*/
int main(int argc, char **argv)
{
	char *dev[15]={"/dev/ttyS1", "/dev/ttyS2", "/dev/ttyS3", "/dev/ttyS4", "/dev/ttyS5", "/dev/ttyS6", "/dev/ttyS7", "/dev/ttyS8", "/dev/ttyS9", "/dev/ttymxc0", "/dev/ttymxc1", "/dev/ttymxc2", "/dev/ttymxc3", "/dev/ttymxc4", "/dev/ttymxc5"};
    int i;
    int nread = 0;
    int fd, c;

    c = atof(argv[1]);

    if(c == 1)
        fd = open_port(dev[0]);
    else if (c == 2)
        fd = open_port(dev[1]);
    else if (c == 3)
        fd = open_port(dev[2]);
    else if (c == 4)
        fd = open_port(dev[3]);
    else if (c == 5)
        fd = open_port(dev[4]);
    else if (c == 6)
        fd = open_port(dev[5]);
    else if (c == 7)
        fd = open_port(dev[6]);
    else if (c == 8)
        fd = open_port(dev[7]);
    else if (c == 9)
        fd = open_port(dev[8]);
    else if (c == 10)
        fd = open_port(dev[9]);
	else if (c == 11)
        fd = open_port(dev[10]);
	else if (c == 12)
        fd = open_port(dev[11]);
	else if (c == 13)
        fd = open_port(dev[12]);
	else if (c == 14)
        fd = open_port(dev[13]);
	else if (c == 15)
        fd = open_port(dev[14]);
    else
    {
        printf("open error!");
        exit(1);
    }

    
    while(1)
    {
        memset(buff, 0, sizeof(buff));
        nread=read(fd,buff,20);
        printf("nread=%d,buff = %s\n",nread,buff);
		
		printf("wait 3\n");
		sleep(1);
		printf("wait 2\n");
		sleep(1);
		printf("wait 1\n");
		sleep(1);
		
        write(fd, buff, strlen(buff));
        sleep(2);
    }

    return 0;

}
