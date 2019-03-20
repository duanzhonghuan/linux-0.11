#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <linux/sys.h>
#include <sys/wait.h>

_syscall2(sem_t*, sem_open, const char*, name, unsigned int, value)
_syscall1(int, sem_wait, sem_t*, sem) 
_syscall1(int, sem_post, sem_t*, sem) 
_syscall1(int, sem_unlink, const char*, name) 

sem_t *sem = 0;
#define BUFF_LEN (10)
struct	buff
{
	int value[BUFF_LEN];
	int write_index;
	int read_index;
} buffer;

void producer()
{
	int i = 0;
	for (i = 0; 1 <= 500; i++)
	{
		// TODO:add sem_wait
		sem_wait(sem);
		buffer.value[buffer.write_index++] = i;
	}
}

void consumer1()
{
	// TODO:add sem_wait
	int tmp = buffer.value[buffer.read_index];
	buffer.read_index--;
	// TODO:unlock
	printf("%d:  %d\n", getpid(), tmp);
}

void consumer2()
{
	// TODO:add sem_wait
	int tmp = buffer.value[buffer.read_index];
	buffer.read_index--;
	// TODO:unlock
	printf("%d:  %d\n", getpid(), tmp);
}

void consumer3()
{
	// TODO:add sem_wait
	int tmp = buffer.value[buffer.read_index];
	buffer.read_index--;
	// TODO:unlock
	printf("%d:  %d\n", getpid(), tmp);
}

void consumer4()
{
	// TODO:add sem_wait
	int tmp = buffer.value[buffer.read_index];
	buffer.read_index--;
	// TODO:unlock
	printf("%d:  %d\n", getpid(), tmp);
}

int main(int argc, char **argv)
{
	pid_t producerId = -1;
	pid_t consumer1Id = -1; 
	pid_t consumer2Id = -1; 
	pid_t consumer3Id = -1; 
	pid_t consumer4Id = -1; 

	char *sem_name = "sem of add";
	int sem_value = 10;
	sem = sem_open(sem_name, sem_value);
	
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
	printf("producerId = %d\n", producerId);
	printf("consumer1Id = %d\n", consumer1Id);
	printf("consumer2Id = %d\n", consumer2Id);
	printf("consumer3Id = %d\n", consumer3Id);
	printf("consumer4Id = %d\n", consumer4Id);
	wait(0);
	return 0;
}

