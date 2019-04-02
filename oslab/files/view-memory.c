/*
 * =====================================================================================
 *
 *       Filename:  view-memory.c
 *
 *    Description:  view the memory by debug
 *
 *        Version:  1.0
 *        Created:  2019年03月29日 17时45分04秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>

int i = 0x12345678;
int main(void)
{
	printf("The logic or virtual address of i is 0x%08x\n, &i");
	while(i);
	return 0;
}
