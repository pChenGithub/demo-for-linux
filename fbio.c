#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>


int main(int argc, char *argv[] ){
    int fd;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    int with;
    int height;
    int bpp;
    char *p;
    int screensize;
    int i,j;
    int *pp;

    printf("fb test\n");
    
    fd = open("/dev/fb0", O_RDWR);
    if(fd<0){
        perror("open error\n");
        return -1;
    }

    if( ioctl(fd, FBIOGET_FSCREENINFO, &finfo)<0 )
    {
        perror("get fix error\n");
        return -1;
    }

    if( ioctl(fd, FBIOGET_VSCREENINFO, &vinfo)<0 )
    {
        perror("get var error\n");
        return -1;
    }

    with = vinfo.xres;
    height = vinfo.yres;
    bpp = vinfo.bits_per_pixel;

    printf("screen %dx%d bpp %d\n", with, height, bpp);

    screensize = with*height*bpp/8;
    p=(char*)mmap(0, screensize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(p==NULL)
    {
        perror("mmap error\n");
        return -1;
    }

    pp = (int*)p;

    for(i=0;i<height*with;i++)
    {
        if(i<with*height/3){
                *pp = 0x000000ff;
        }else if(i<2*with*height/3){
                *pp = 0x0000ff00;
        }else{
                *pp = 0x00ff0000;
        }
                pp++;
    }

    munmap((void*)p, screensize);
    close(fd);
    return 0;
}
