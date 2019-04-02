/*
 * =====================================================================================
 *
 *       Filename:  shm.c
 *
 *    Description:  share the memory between multiple processes.
 *
 *        Version:  1.0
 *        Created:  2019年04月01日 23时32分24秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (zhonghuan duan), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <linux/mm.h>
#include <linux/sched.h>
#include <errno.h>

#define COUNT (10)
#define SIZE_MAX  (4096)
unsigned long share_physical_address[COUNT] = {0};

/* create or open a page of the share memroy */
int sys_shmget(int key, size_t size, int shmflg)
{
	// forbid it that more than 4K size
	if (key < 0 || key >= COUNT || size > SIZE_MAX)
	{
		errno = EINVAL; 
		return -1;
	}

	// allocate a page of the free page from the physical memroy
	if (0 != share_physical_address[key])
	{
		share_physical_address[key] = get_free_page();
		if (0 == share_physical_address[key])
		{
			errno = EINVAL; 
			return -1;
		}
	}
	return share_physical_address[key];
}

/* get logic address of the memory address */
int sys_shmat(int shmid, const void *shmaddr, int shmflg)
{
	unsigned long data_base = 0; 
	unsigned long data_limit = 0; 
	if (shmid == -1)
	{
		errno = EINVAL;
		return 0;
	}
	
	// establish a mapping between the physical page and the current virtual breakpoint 
	put_page(shmid, (current->start_code + current->brk));

	return current->brk;
}

