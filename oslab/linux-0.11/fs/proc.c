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
#include <asm/segment.h>

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
char *hdinfo = 0;

static int update_psinfo(off_t * pos, char * buf, int count)
{
	struct task_struct **p = 0;
	int offset = 0;
	int i = 0;
	static int flag = 0;
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
	// copy the data to user space
	for (i = 0; i < offset; i++)
	{
		put_fs_byte(psinfo[i], buf+i);
	}
	if (flag == 0)
	{
		flag = 1;
		return offset;
	}
	flag = 0;
	free(psinfo);
	psinfo = 0;
	return 0;
}

int proc_read(int dev, off_t * pos, char * buf, int count)
{
	// read current ps info
	if (dev == PS_DEV)
	{
		return update_psinfo(pos, buf, count);
	}
	
	int offset = 0;
	int i = 0;
	static int flag = 0;
	// read current hd info
	if (dev == HD_DEV)
	{
		if (flag == 1)
		{
			flag = 0;
			return 0;
		}
		struct super_block *s = 0;
		s = get_super(dev);
		hdinfo = malloc(512);
		offset += sprintf(hdinfo + offset, "s_ninodes:         %d;\n", s->s_ninodes);
		offset += sprintf(hdinfo + offset, "s_nzones:          %d;\n", s->s_nzones);
		offset += sprintf(hdinfo + offset, "s_imap_blocks:     %d;\n", s->s_imap_blocks);
		offset += sprintf(hdinfo + offset, "s_zmap_blocks:     %d;\n", s->s_zmap_blocks);
		
		// copy the data to user space
		for (i = 0; i < offset; i++)
		{
			put_fs_byte(hdinfo[i], buf+i);
		}
		free(hdinfo);
		hdinfo = 0;
		flag = 1;
		return offset;
	}

	return 0;
}

