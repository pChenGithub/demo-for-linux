#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <limits.h>
#include <asm/ioctls.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

int openSerial(const char *cSerialName)
{

    struct termios opt;
    int fd = open(cSerialName, O_RDWR  );
    if(fd < 0)
    {
        perror(cSerialName);
        return -1;
    }

    //printf(" 16 fd =  %d\n",fd);
    tcgetattr(fd, &opt);
    tcflush(fd,  TCIOFLUSH);
    opt.c_cflag   |=   CS8;
    opt.c_cc[VMIN]   =   0;
    opt.c_cc[VTIME]  =   0;

    opt.c_cflag &= ~PARENB;
    opt.c_cflag &= ~CSTOPB;
    opt.c_cflag &= ~CSIZE;
    opt.c_cflag |= CS8;
    opt.c_cflag &= ~CSTOPB;
    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); //~ICANON;

    opt.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    opt.c_oflag     &= ~OPOST;
    //opt.c_cflag |= CRTS_IFLOW;
    


    cfsetispeed(&opt, B9600);
    cfsetospeed(&opt, B9600);
    //SetupComm(fd,4096,4096);

    if (tcsetattr(fd,   TCSADRAIN,   &opt)<0) {
        return   -1;
    }
    printf("its ok");
    return fd; 
}

int main()
{
    int fd = openSerial("/dev/ttymxc1");
    if(fd > -1)
    {
        printf("open ok\n");
    }
    char data[10] = "";
    
    while(1)
    {
        int ret = read(fd,data,10);
        perror("\nread");
        printf("data = %s ret = %d\n",data,ret);
        sleep(2);
    }

}
