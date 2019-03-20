#include <linux/sched.h>
#include <linux/kernel.h>
#include <asm/system.h>
#include <string.h>

#define MAX_NAME_LEN  (32)
#define MAX_SEM_NUM  (64)

struct sem_t {
	char  name[MAX_NAME_LEN];
	int max_value;
	int value;
	struct task_struct	*b_wait;
	int enable;
	int lock;
};

struct sem_list_info {
	struct sem_t *sem_list[MAX_SEM_NUM];
	int index;
};

struct sem_list_info sem_info = {0};

static int find_sem(const char *name)
{
	int i = 0;
	for (i = 0; i < MAX_SEM_NUM; i++)
	{
		if (0 == strcmp(sem_info.sem_list[i]->name, name))
		{
			return i;
		}
		i++;
	}
	return -1;
}

sem_t *sys_sem_open(const char *name, unsigned int value)
{
	sem_t *sem = 0;
	int i = 0;
	int index = find_sem(name);
	cli();
	if (-1 == index)  // new sem, need create it
	{
		for (i = 0; i < MAX_SEM_NUM; i++)
		{
			if (!sem_info.sem_list[i])
			{
				sem_info.sem_list[i] = malloc(sizeof(struct sem_t)); 
				sem_info.sem_list[i].max_value = value;
				sem_info.sem_list[i].value = value;
				sem_info.sem_list[i].enable = 1;
				sem = sem_info.sem_list[i];
				break;
			}
		}
	}
	else
	{
		sem_info.sem_list[index].enable = 1;
		sem = sem_info.sem_list[index];
	}
	sti();
	return sem;
}

int sys_sem_wait(sem_t *sem)
{
	int i = 0;
	cli();
	if (--sem->value < 0) // if the value is less than 0, save the info and enter the schedule
	{
		sleep_on(*(sem->b_wait);
	}
	else
	{
		i = -1;
	}
	sti();
	return i;
}

int sys_sem_post(sem_t *sem)
{
	int i = 0;
	cli();
	if (sem->value + 1 <= sem->max_value)
	{
		sem->value++;
		wake_up(*(sem->b_wait);
	}
	else
	{
		i = -1;
	}
	sti();
	return i;
}

int sys_sem_unlink(const char *name)
{
	int i = 0;
	int index = find_sem(name);
	cli();
	if (index >= 0)
	{
		free(sem_info.sem_list[index]);
	}
	else
	{
		i = -1;
	}
	sti();
	return i;
}
