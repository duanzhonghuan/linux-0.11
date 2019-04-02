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
int *value_index = 0;
int main(int argc, char **argv)
{
#define BUFF_LEN (10)
#define MAX_SIZE (500)

	int shmid = 0;
	int share_memory_address = 0;
	sem_t *sem_empty = NULL;
	sem_t *sem_full = NULL;
	sem_t *sem_mutex = NULL;
	int i = 0;
	FILE *result = NULL;

	result = fopen("/var/result.txt", "wb+");
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
	
	value_index = (int*)(share_memory_address + 16);


	sem_empty = (sem_t *)sem_open("EMPTY", BUFF_LEN);
	sem_full  = (sem_t *)sem_open("FULL", 0);
	sem_mutex = (sem_t *)sem_open("MUTEX", 1);
	for (i = 0; 1 <= MAX_SIZE; i++)
	{
		sem_wait(sem_empty);  
		sem_wait(sem_mutex); 
		
		buff[*value_index] = i;
		fprintf(result, "write: %d\n", i);
		fflush(result);
		*value_index = (*value_index + 1) % BUFF_LEN;

		sem_post(sem_mutex);
		sem_post(sem_full);
	}

	sem_unlink("EMPTY");
	sem_unlink("FULL");
	sem_unlink("MUTEX");
	fclose(result);
	
	return 0;
}

