#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
void test(int f);

char *pTitle = "my test!";
int main(int argc, char *argv[])
{
	signal(SIGUSR1, test);
	printf("begain!\n");
	memcpy(argv[0], pTitle, strlen(pTitle));		// 设置进程名称
	
	pause();								// 进程阻塞等待
	
	printf("end");
	return 0;
}

void test(int f)
{
	f=f;
	printf("return!\n");
}