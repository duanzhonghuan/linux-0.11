#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

int sem_open(const char* name,unsigned int value) 
{ 
	long __res; 
	__asm__ volatile ("int $0x80" 
		: "=a" (__res) 
		: "0" (72),"b" ((long)(name)),"c" ((long)(value))); 
	if (__res >= 0) 
		return (int) __res; 
	errno = -__res; 
	return -1; 
}

int sem_wait(sem_t* sem) 
{ 
	long __res; 
	__asm__ volatile ("int $0x80" 
		: "=a" (__res) 
		: "0" (73),"b" ((long)(sem))); 
	if (__res >= 0) 
		return (int) __res; 
	errno = -__res; 
	return -1; 
}

int sem_post(sem_t* sem) 
{ 
	long __res; 
	__asm__ volatile ("int $0x80" 
		: "=a" (__res) 
		: "0" (74),"b" ((long)(sem))); 
	if (__res >= 0) 
		return (int) __res; 
	errno = -__res; 
	return -1; 
}

int sem_unlink(const char* name) 
{ 
	long __res; 
	__asm__ volatile ("int $0x80" 
		: "=a" (__res) 
		: "0" (75),"b" ((long)(name))); 
	if (__res >= 0) 
		return (int) __res; 
	errno = -__res; 
	return -1; 
}

int shmget(int key, size_t size,int shmflg) 
{ 
	long __res; 
	__asm__ volatile ("int $0x80" 
		: "=a" (__res) 
		: "0" (76),"b" ((long)(key)),"c" ((long)(size)),"d" ((long)(shmflg))); 
	if (__res>=0) 
		return (int) __res; 
	errno=-__res; 
	return -1; 
}

int shmat(int shmid, const void * shmaddr, int shmflg) 
{ 
	long __res; 
	__asm__ volatile ("int $0x80" 
		: "=a" (__res) 
		: "0" (77),"b" ((long)(shmid)),"c" ((long)(shmaddr)),"d" ((long)(shmflg))); 
	if (__res>=0) 
		return (int) __res; 
	errno=-__res; 
	return -1; 
}


void write_buff(FILE *fl, int data, int pos)
{
	fseek(fl, 4 * pos, SEEK_SET);
	fwrite(&data, sizeof(int), 1, fl);
	fflush(fl);
}

void read_buff(FILE *fl, int *data, int pos)
{
	fseek(fl, 4 * pos, SEEK_SET);
	fread(data, sizeof(int), 1, fl);
}

char *buff = 0;
int main(int argc, char **argv)
{
#define BUFF_LEN (10)
#define MAX_SIZE (120)

	int shmid = 0;
	int share_memory_address = 0;
	sem_t *sem_empty = NULL;
	sem_t *sem_full = NULL;
	sem_t *sem_mutex = NULL;
	int i = 0;
	int index = 0;
	int count = 0;
	FILE *result = NULL;
	int *end_flag = 0;
	int data = 0;

	result = fopen("/var/restt", "a");
	if (result == NULL)
	{
		printf("can not open result buff by wb+ \n");
		return 0;
	}
	
	shmid = shmget(0, 0, 0);
	if (shmid == -1)
	{
		printf("can not allocate the share memory \n");
		return 0;
	}
	
	share_memory_address = shmat(shmid, 0, 0);
	if (share_memory_address == 0)
	{
		printf("can not allocate the virtual memory \n");
		return 0;
	}

	buff = (char*)share_memory_address;

	end_flag = (char*)(share_memory_address + 12);
	
	data = *(int*)(share_memory_address + 22);
	printf("data: %d \n", data);

	sem_empty = (sem_t *)sem_open("EMPTY", BUFF_LEN);
	sem_full  = (sem_t *)sem_open("FULL", 0);
	sem_mutex = (sem_t *)sem_open("MUTEX", 1);
	for (;;)
	{
		sem_wait(sem_full);
		sem_wait(sem_mutex);
		fseek(result, 2, SEEK_END);
		fprintf(result, "pid:%d:  read data = %d, read index = %d\n", getpid(), buff[index], index);
		fflush(result);
		
		index = (index + 1) % BUFF_LEN;

		count++;
		if (count >= MAX_SIZE + 1)
		{
			*end_flag = 123;
			printf("consumer:end_flag = %d\n", *(char*)(share_memory_address + 12));
			break;
		}

		sem_post(sem_mutex);
		sem_post(sem_empty);
	}

	printf("consumer end\n");
	return 0;
}

