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

#include <linux/kernel.h>
#include <linux/mm.h>
#include <sched.h>
#include <errno.h>

typedef int key_t;
int shmid = -1;
unsigned long share_physical_address = 0;

/* create or open a page of the share memroy */
int shmget(key_t key, size_t size, int shmflg)
{
	// forbid it that more than 4K size
	if (key <= 0 || size > 0x1000)
	{
		errno = EINVAL; 
		return -1;
	}
	if (-1 == shmid)
	{
		share_physical_address = get_free_page();
		if (0 == share_physical_address)
		{
			errno = EINVAL; 
			return -1;
		}
		shmid = 1;
	}
	return shmid;
}

/* get logic or virtual address from the physical address */
static void *get_logic_address(unsigned long physical_address)
{

}

/* get logic address of the memory address */
void *shmat(int shmid, const void *shmaddr, int shmflg)
{
	if (shmid == -1)
	{
		errno = EINVAL;
		return (void*)0;
	}
	return get_logic_address(share_physical_address);
}
