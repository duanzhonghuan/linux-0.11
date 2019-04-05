#include <linux/sched.h>
#include <linux/kernel.h>
#include <asm/system.h>
#include <asm/segment.h>
#include <string.h>
#include <stdarg.h>

#define MAX_NAME_LEN  (32)
#define MAX_SEM_NUM  (64)

struct sem_s {
	char  name[MAX_NAME_LEN];
	int max_value;
	int value;
	struct task_struct	*b_wait;
	int enable;
};
typedef struct sem_s sem_t;


struct sem_list_info {
    sem_t *sem_list[MAX_SEM_NUM];
	int index;
};

struct sem_list_info sem_info = {0};

static void get_fs_buff(const char *name, char*buff, int size)
{
	int i;
	memset(buff, 0, size);
	for (i = 0; i < size; i++)
	{
		buff[i] = get_fs_byte(name++);
		if (buff[i] == '\0')
		{
			break;
		}
	}
}

static int find_sem(const char *name, char *buf)
{
	int i = 0;
	get_fs_buff(name, buf, 1024);
	for (i = 0; i < MAX_SEM_NUM; i++)
	{
		if (!sem_info.sem_list[i])
		{
			continue;
		}
		printk("find_sem: sem_info.sem_list[i]->name = %s\n", sem_info.sem_list[i]->name);
		if (0 == strcmp(sem_info.sem_list[i]->name, buf))
		{
			printk("find_sem: buf = %s\n", buf);
			return i;
		}
	}
	return -1;
}



int sys_sem_open(const char *name, unsigned int value)
{
	sem_t *sem = 0;
	int i = 0;
	char buf[1024] = {0};
	int index = find_sem(name, buf);
	printk("sys_sem_open: %s\n", buf);
	cli();
	if (-1 == index)  // new sem, need create it
	{
		for (i = 0; i < MAX_SEM_NUM; i++)
		{
			if (!sem_info.sem_list[i])
			{
				sem_info.sem_list[i] = malloc(sizeof(sem_t)); 
				sem_info.sem_list[i]->max_value = value;
				sem_info.sem_list[i]->value = value;
				sem_info.sem_list[i]->enable = 1;
				strcpy(sem_info.sem_list[i]->name, buf);
				printk("sys_sem_open: %s, i = %d\n", buf, i);
				sem = sem_info.sem_list[i];
				index = i;
				break;
			}
		}
	}
	else
	{
		sem_info.sem_list[index]->enable = 1;
		sem = sem_info.sem_list[index];
	}
	sti();
	printk("sys_sem_open: %s, index = %d\n", sem_info.sem_list[i]->name, index);
	return sem;
}

int sys_sem_wait(sem_t *sem)
{
	if (!sem || !sem->enable) {return -1;}
	int i = 0;
	cli();
	if (--sem->value < 0)  // if the value is less than 0, save the info and enter the schedule
	{
		sleep_on(&(sem->b_wait));
	}
	sti();
	return i;
}

int sys_sem_post(sem_t *sem)
{
	if (!sem || !sem->enable) {return -1;}
	int i = 0;
	cli();
	sem->value++;
	wake_up(&(sem->b_wait));
	sti();
	return i;
}

int sys_sem_unlink(const char *name)
{
	int i = 0;
	char buf[1024] = {0};
	int index = find_sem(name, buf);
	cli();
	if (index >= 0 && sem_info.sem_list[index])
	{
		free(sem_info.sem_list[index]);
		printk("sys_sem_unlink: %s\n", buf);
	}
	sti();
	return i;
}

