#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <linux/sys.h>
#include <sys/wait.h>

_syscall2(sem_t*, sem_open, const char*, name, unsigned int, value)
_syscall1(int, sem_wait, sem_t*, sem) 
_syscall1(int, sem_post, sem_t*, sem) 
_syscall1(int, sem_unlink, const char*, name) 

#define BUFF_LEN (10)
#define MAX_SIZE (500)
struct	buff
{
	int write_index;
	int read_index;
	int read_count;
} buffer;

sem_t *sem_empty = NULL;
sem_t *sem_full = NULL;
sem_t *sem_mutex = NULL;
FILE *fl = NULL;


void write_buff(FILE *fl, int data, int pos)
{
	fseek(fl, szieof(int) * pos, SEEK_SET);
	fwrite(&data, sizeof(int), 1, fl);
	fflush(fl);
}

void read_buff(FILE *fl, int *data, int pos)
{
	fseek(fl, szieof(int) * pos, SEEK_SET);
	fread(data, sizeof(int), 1, fl);
}

void producer()
{
	int i = 0;
	for (i = 0; 1 <= MAX_SIZE; i++)
	{
		sem_wait(sem_empty);  // 10 choose 1
		sem_wait(sem_mutex);  // 5(1 producee and 4 consumers) choose 1
		
		write_buff(fl, i, buffer.write_index);
		buffer.write_index = (buffer.write_index + 1) % BUFF_LEN;

		sem_post(sem_mutex);
		sem_post(sem_full);
	}
}

void consumer1()
{
	int data;
	for (;;)
	{
		sem_wait(sem_full);
		sem_wait(sem_mutex);

		read_buff(fl, &data, buffer.read_index);
		buffer.read_index = (buffer.read_index + 1) % BUFF_LEN;
		printf("%d:  %d\n", getpid(), data);
		fflush(stdout);
		
		// exit it when read all data from the file
		buffer.read_count++;
		if (buffer.read_count >= MAX_SIZE)
		{
			break;
		}

		sem_post(sem_mutex);
		sem_post(sem_empty);
	}
}

void consumer2()
{
	int data;
	for (;;)
	{
		sem_wait(sem_full);
		sem_wait(sem_mutex);

		read_buff(fl, &data, buffer.read_index);
		buffer.read_index = (buffer.read_index + 1) % BUFF_LEN;
		printf("%d:  %d\n", getpid(), data);
		fflush(stdout);

		// exit it when read all data from the file
		buffer.read_count++;
		if (buffer.read_count >= MAX_SIZE)
		{
			break;
		}

		sem_post(sem_mutex);
		sem_post(sem_empty);
	}
}

void consumer3()
{
	int data;
	for (;;)
	{
		sem_wait(sem_full);
		sem_wait(sem_mutex);

		read_buff(fl, &data, buffer.read_index);
		buffer.read_index = (buffer.read_index + 1) % BUFF_LEN;
		printf("%d:  %d\n", getpid(), data);
		fflush(stdout);

		// exit it when read all data from the file
		buffer.read_count++;
		if (buffer.read_count >= MAX_SIZE)
		{
			break;
		}

		sem_post(sem_mutex);
		sem_post(sem_empty);
	}
}

void consumer4()
{
	int data;
	for (;;)
	{
		sem_wait(sem_full);
		sem_wait(sem_mutex);

		read_buff(fl, &data, buffer.read_index);
		buffer.read_index = (buffer.read_index + 1) % BUFF_LEN;
		printf("%d:  %d\n", getpid(), data);
		fflush(stdout);

		// exit it when read all data from the file
		buffer.read_count++;
		if (buffer.read_count >= MAX_SIZE)
		{
			break;
		}

		sem_post(sem_mutex);
		sem_post(sem_empty);
	}
}

int main(int argc, char **argv)
{
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

	if ((producerId = fork()) == 0)
	{
		producer();
	}
	else if ((consumer1Id = fork()) == 0)
	{
		consumer1();
	}
	else if ((consumer2Id = fork()) == 0)
	{
		consumer2();
	}
	else if ((consumer3Id = fork()) == 0)
	{
		consumer3();
	}
	else if ((consumer4Id = fork()) == 0)
	{
		consumer4();
	}
	wait(NULL);
	sem_unlink("EMPTY");
	sem_unlink("FULL");
	sem_unlink("MUTEX");
	
	printf("producerId = %d\n", producerId);
	printf("consumer1Id = %d\n", consumer1Id);
	printf("consumer2Id = %d\n", consumer2Id);
	printf("consumer3Id = %d\n", consumer3Id);
	printf("consumer4Id = %d\n", consumer4Id);
	fflush(stdout);
	return 0;
}

