#define __LIBRARY__
#include <unistd.h>
#include <asm/segment.h>
#include <asm/system.h>
#include <signal.h>
#include <linux/sched.h>
#include <linux/kernel.h>


int sys_sem_open(const char *name, unsigned int value)
{

    return -1;
}

int sys_sem_wait(sem_t* sem)
{
    cli();

    sti();
    return 0;
}

int sys_sem_post(sem_t * sem)
{
    cli();



    sti();
    return 0;
}


int sys_sem_unlink(const char *name)
{

    return -1;
}