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

int main(int argc, char **argv)
{
#define BUFF_LEN (10)
#define MAX_SIZE (500)
#define WRITE_POS (0)
#define READ_POS (1)
#define COUNT_POS (2)
	
	int i = 0;
	int data;
	int index;
	int count;
	sem_t *sem_empty = NULL;
	sem_t *sem_full = NULL;
	sem_t *sem_mutex = NULL;
	FILE *fl = NULL;
	FILE *result = NULL;
	FILE *file_value = NULL;
	
	pid_t producerId = -1;
	pid_t consumer1Id = -1; 
	pid_t consumer2Id = -1; 
	pid_t consumer3Id = -1; 
	pid_t consumer4Id = -1; 

	sem_empty = (sem_t *)sem_open("EMPTY", BUFF_LEN);
	sem_full  = (sem_t *)sem_open("FULL", 0);
	sem_mutex = (sem_t *)sem_open("MUTEX", 1);

	fl = fopen("/var/buff.txt", "wb+");
	if (fl == NULL)
	{
		printf("can not open buff by wb+ \n");
		return 0;
	}

	result = fopen("/var/result.txt", "wb+");
	if (result == NULL)
	{
		printf("can not open result buff by wb+ \n");
		return 0;
	}
	
	file_value = fopen("/var/filevalue.txt", "wb+");
	if (file_value == NULL)
	{
		printf("can not open result buff by wb+ \n");
		return 0;
	}
	write_buff(file_value, 0, WRITE_POS);
	write_buff(file_value, 0, READ_POS);
	write_buff(file_value, 0, COUNT_POS);

	if ((producerId = fork()) == 0)
	{
		for (i = 0; 1 <= MAX_SIZE; i++)
		{
			sem_wait(sem_empty);  
			sem_wait(sem_mutex); 
			
			read_buff(file_value, &index, WRITE_POS);
					
			write_buff(fl, i, index);
			fprintf(result, "write: %d\n", i);
			fflush(result);
			
			index = (index + 1) % BUFF_LEN;
			write_buff(file_value, index, WRITE_POS);

			sem_post(sem_mutex);
			sem_post(sem_full);
		}
		exit(0);
	}
	else if ((consumer1Id = fork()) == 0)
	{
		for (;;)
		{
			sem_wait(sem_full);
			sem_wait(sem_mutex);

			read_buff(file_value, &index, READ_POS);
			
			read_buff(fl, &data, index);
			fprintf(result, "pid1:%d:  read data = %d, read index = %d\n", getpid(), data, index);
			fflush(result);
			
			index = (index + 1) % BUFF_LEN;
			write_buff(file_value, index, READ_POS);
			
			read_buff(file_value, &count, COUNT_POS);
			count++;
			write_buff(file_value, count, COUNT_POS);

			if (count >= MAX_SIZE)
			{
				break;
			}

			sem_post(sem_mutex);
			sem_post(sem_empty);
		}
		exit(0);
	}
	else if ((consumer2Id = fork()) == 0)
	{
		for (;;)
		{
			sem_wait(sem_full);
			sem_wait(sem_mutex);

			read_buff(file_value, &index, READ_POS);
			
			read_buff(fl, &data, index);
			fprintf(result, "pid2:%d:  read data = %d, read index = %d\n", getpid(), data, index);
			fflush(result);
			
			index = (index + 1) % BUFF_LEN;
			write_buff(file_value, index, READ_POS);
			
			read_buff(file_value, &count, COUNT_POS);
			count++;
			write_buff(file_value, count, COUNT_POS);

			if (count >= MAX_SIZE)
			{
				break;
			}

			sem_post(sem_mutex);
			sem_post(sem_empty);
		}
		exit(0);
	}
	else if ((consumer3Id = fork()) == 0)
	{
		for (;;)
		{
			sem_wait(sem_full);
			sem_wait(sem_mutex);

			read_buff(file_value, &index, READ_POS);
			
			read_buff(fl, &data, index);
			fprintf(result, "pid3:%d:  read data = %d, read index = %d\n", getpid(), data, index);
			fflush(result);
			
			index = (index + 1) % BUFF_LEN;
			write_buff(file_value, index, READ_POS);
			
			read_buff(file_value, &count, COUNT_POS);
			count++;
			write_buff(file_value, count, COUNT_POS);

			if (count >= MAX_SIZE)
			{
				break;
			}

			sem_post(sem_mutex);
			sem_post(sem_empty);
		}
		exit(0);
	}
	else if ((consumer4Id = fork()) == 0)
	{
		for (;;)
		{
			sem_wait(sem_full);
			sem_wait(sem_mutex);

			read_buff(file_value, &index, READ_POS);
			
			read_buff(fl, &data, index);
			fprintf(result, "pid4:%d:  read data = %d, read index = %d\n", getpid(), data, index);
			fflush(result);
			
			index = (index + 1) % BUFF_LEN;
			write_buff(file_value, index, READ_POS);
			
			read_buff(file_value, &count, COUNT_POS);
			count++;
			write_buff(file_value, count, COUNT_POS);

			if (count >= MAX_SIZE)
			{
				break;
			}

			sem_post(sem_mutex);
			sem_post(sem_empty);
		}
		exit(0);
	}
	wait(NULL);
	sem_unlink("EMPTY");
	sem_unlink("FULL");
	sem_unlink("MUTEX");
	
	fclose(fl);
	fclose(result);
	fclose(file_value);
	return 0;
}

