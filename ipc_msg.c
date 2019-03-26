#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include<errno.h>
#include<string.h>
#include <sys/types.h>
#include <unistd.h>
struct msg_buf
{
    long mtype;
    char data[255];
};

/* 注意long 和int在32bit 和 64bit系统之下是不一样的
structmsg_buf
{
    long mtype;
    char data[255];
};
*/

int main()
{
    key_t key;
    int msgid;
    int ret;
    struct msg_buf    msgbuf;
    int    msgtype =  getpid();
    //系统建立IPC通讯 （消息队列、信号量和共享内存） 时必须指定一个ID值。通常情况下，该id值通过ftok函数得到。
    key=ftok("./msgfile",'a');
    printf("key =[%x]\n",key);

    printf("sizeof(long):%ld, sizeof(int):%d \n", sizeof(long), sizeof(int));
    //用来创建和访问一个消息队列
    msgid=msgget(key, IPC_CREAT |IPC_EXCL|0666); //通过文件对应

    if(msgid==-1)
    {
        if (errno == EEXIST)
        {
            printf("EEXIST:.....\n");
            key=ftok("./msgfile",'a');
            msgid=msgget(key, IPC_CREAT|0666); //通过文件对应
        }
        else
        {
            printf("create error\n");
            perror("msget: \n");
            return -1;
        }
        
    }
    printf("msgid:%d \n", msgid);
    
    msgbuf.mtype = msgtype; //        getpid();

    printf("getpid(): %d \n", getpid());
    strcpy(msgbuf.data,"hello world!");
    //把一条data消息添加到消息队列,IPC_NOWAIT表示队列满不等待，返回EAGAIN错误
    ret = msgsnd(msgid,&msgbuf, sizeof(msgbuf.data), IPC_NOWAIT);
    if(ret==-1)
    {
        printf("send message err\n");
        perror("senderr");
        return -1;
    }
    sleep(1);

    memset(&msgbuf,0,sizeof(msgbuf));
    //是从一个消息队列接收消息,队列没有可读消息不等待，返回ENOMSG错误
    ret=msgrcv(msgid, &msgbuf, sizeof(msgbuf.data), msgtype, IPC_NOWAIT);
    if(ret==-1)
    {
        printf("recv message err\n");
        perror("dd");
        return -1;
    }
    printf("recvmsg =[%s]\n",msgbuf.data);
    return 0;
}
