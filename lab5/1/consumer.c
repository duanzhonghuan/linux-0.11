#define   __LIBRARY__  
#include  <unistd.h>  
#include  <stdio.h>
#define N 4096

#define M 500

_syscall2(int, shmget, int, key,int ,size);
_syscall1(void *, shmat, int, shmid);

int main()
{
	int i,shmid,*num;
	shmid=shmget(1,N);
	num=(int *)shmat(shmid);
	for(i=0;i<M;i++)
	{
		printf("%3d ",num[i]);
	}
	return 0;
}
