#define __LIBRARY__  
#include <unistd.h>  
#include <linux/sched.h>  
#include <linux/kernel.h>  
#include <asm/segment.h>  
#include <asm/system.h>  
#include <signal.h> 
#define N 4096
int vector[40]={0};

int sys_shmget(int key,int size)
{
	int res;

	if(vector[key]!=0)
	{
		return vector[key];
	}

	if(size>N)
	{
		return -1;
	}

	res=get_free_page();
	if(!res)
	{
		return -1;
	}
	vector[key]=res;
	return res;	
}

void *sys_shmat(int shmid){
	if(!shmid)
	{
		return -1;
	}
	put_page(shmid,current->start_code+current->brk);
	return current->brk;
}
