/*************************************************************************
	> File Name: daemon.c
	> Author: chen.peng
	> Mail: 404285202@qq.com 
	> Created Time: 2019年05月05日 星期日 15时58分59秒
 ************************************************************************/

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>


char processExists(char * process_name) {
	FILE *ptr;
	int RE_BUF_SIZE = 32;
	char rebuff[RE_BUF_SIZE];
	char ps[128];
	snprintf(ps, sizeof(ps), "ps | grep %s | wc -l", process_name);
	if((ptr = popen(ps, "r")) != NULL) {
		int count = 0;
		fgets(rebuff, RE_BUF_SIZE, ptr);
		if(rebuff != NULL) {
			count = atoi(rebuff);
		}
		pclose(ptr);
		return count >= 1;
	}
	return -1;
}

char start_process(char* process_name) {

	FILE *ptr;
	int RE_BUF_SIZE = 32;
	char rebuff[RE_BUF_SIZE];
	char ps[128];
	snprintf(ps, sizeof(ps), "%s", process_name);

	if((ptr = popen(ps, "r")) != NULL) {
/*		fgets(rebuff, RE_BUF_SIZE, ptr);
		if(rebuff != NULL) {
			count = atoi(rebuff);
		}*/
		pclose(ptr);
		return 0;
	}

	return -1;
}

int main() {

	int pid;


	signal(SIGINT,	SIG_IGN);// 终端中断  
	signal(SIGHUP,	SIG_IGN);// 连接挂断  
	signal(SIGQUIT, SIG_IGN);// 终端退出  
	signal(SIGPIPE, SIG_IGN);// 向无读进程的管道写数据	
	signal(SIGTTOU, SIG_IGN);// 后台程序尝试写操作	
	signal(SIGTTIN, SIG_IGN);// 后台程序尝试读操作	
	signal(SIGTERM, SIG_IGN);// 终止

	pid = fork();
	if (pid<0) {
	
		perror("fork error ... \n");
		return -1;
	}else if (pid>0) {
	
		exit(0);
	}

	setsid();

	pid = fork();
	if (pid<0) {
	
		return -1;
	}else if (pid>0) {
	
		exit(0);
	}

	chdir("./");

	umask(0);

	close(0);
	close(1);
	close(2);

	signal(SIGCHLD,SIG_IGN);



	while (1) {

		if (processExists("hello")<=0) {
			start_process("touch xxx.txt");
			start_process("./hello");
		}

		sleep(3);

	}

	return 0;
}
