#define   __LIBRARY__  
#include  <unistd.h>  
#include  <stdio.h>

_syscall2(int, shmget, int, key,int ,size);
_syscall1(void *, shmat, int, shmid);

int main(){
	int* num;
	int i;
	int shmid=shmget(1,4096);
	num=(int *)shmat(shmid);
	for(i=0;i<500;i++){
		printf("%d\n",*(num+i));
	}
	return 0;
}
