#define   __LIBRARY__  
#include  <unistd.h>  
#include  <stdio.h>
#define N 4096

#define M 500
#define EINVAL		22
#define ENOMEM		12

_syscall2(int, shmget, int, key,int ,size);
_syscall1(void *, shmat, int, shmid);

int main()
{
	int i, shmid, *num;
	shmid=shmget(1,N);
	
        if(shmid==-EINVAL) {
                printf("larger than size of one page!");
                
        }
	else if(shmid==-ENOMEM)  
	{
                printf("no free page!"); 
                
        }
        else 
	{
		num=(int *)shmat(shmid);

		for(i=0;i<M;i++)
		{
			*(num+i)=i+1;
		}
	while(1);
	}
	return 0;
}
