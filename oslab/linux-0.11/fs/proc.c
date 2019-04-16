/*
 * =====================================================================================
 *
 *       Filename:  proc.c
 *
 *    Description:  achieve the function of reading the proc inode.  
 *
 *        Version:  1.0
 *        Created:  2019年04月16日 22时24分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (duan zhonghuan), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <sys/types.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <stdarg.h>

#define PS_DEV    (0)
#define HD_DEV    (1)

extern int vsprintf(char *buf, const char *fmt, va_list args);

static int sprintf(char *buf, const char *fmt, ...)
{
    va_list args; int i;
    va_start(args, fmt);
    i=vsprintf(buf, fmt, args);
    va_end(args);
    return i;
}

char* psinfo = 0;

static int update_psinfo(off_t * pos, char * buf, int count)
{
	struct task_struct **p = 0;
	int offset = 0;
	if (psinfo == 0)
	{
		psinfo = malloc(512);
	}
	offset = sprintf(psinfo + offset, "pid state father counter start_time\n");
	for(p = &LAST_TASK ; p > &FIRST_TASK ; --p)
	{
		if (*p == 0)
	   	{
			continue;
		}
		if (offset > 512 - 20)
		{
			break;
		}
		offset += sprintf(psinfo + offset, "%ld %ld %ld %ld %ld \n",
			   	(*p)->pid, (*p)->state, (*p)->father,
			   	(*p)->counter, (*p)->start_time); 
	}
}

int proc_read(int dev, off_t * pos, char * buf, int count)
{
	// read current ps info
	if (dev == PS_DEV)
	{
		return update_psinfo(pos, buf, count);
	}

	// read current hd info
	if (dev == HD_DEV)
	{

	}

	return 0;
}

