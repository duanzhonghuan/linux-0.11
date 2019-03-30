#define NUMBUFFER   500
#define NUMPRO      6
#define MAXSIZE     10
#define N           10

#define   __LIBRARY__
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


_syscall2(int,  sem_open,   const char*, name, unsigned int , value)
_syscall1(int,  sem_wait,   sem_t  *, sem)
_syscall1(int,  sem_post,   sem_t  *, sem)
_syscall1(int,  sem_unlink, const char  *, name)

void num_in(FILE* fp,int num , int pos);

void num_out(FILE* fp,int* num ,int pos);

int main()
{
    int  Cost;
    int  POS_get = 0, POS_put = 0;
    int i,j,num=NUMBUFFER/NUMPRO;

    FILE *fp = NULL;
    sem_t  *EMPTY;
    sem_t  *FULL;
    sem_t  *MUTEX;

    EMPTY = (sem_t *)sem_open("EMPTY",N);
    FULL  = (sem_t *)sem_open("FULL",0);
    MUTEX = (sem_t *)sem_open("MUTEX",1);

    fp=fopen("/var/Buffer.txt", "wb+");
    if(fp==NULL){printf("...i do no know...........");exit(0);}

    num_in(fp,POS_get,N);

    if( !fork() )
    {
        for( i = 0 ; i < NUMBUFFER; i++)
        {
            sem_wait(EMPTY),sem_wait(MUTEX);
            
            num_in(fp,i,POS_put);
            POS_put = ( POS_put + 1)% MAXSIZE;
            
            sem_post(MUTEX),sem_post(FULL);
        }
        exit(0);
    }

    for( i = 0; i < NUMPRO ; i++ )
    {
        if( !fork() )
        {
            for( j = 0; j < num; j++ )
            {
                sem_wait(FULL),sem_wait(MUTEX);
                
                fflush(stdout);
                num_out(fp,&POS_get ,N);
                num_out(fp,&Cost    ,POS_get);
                printf("%d:\t%d\n",getpid(),Cost);
                fflush(stdout);
                POS_get = (POS_get + 1) % MAXSIZE;
                num_in(fp,POS_get,N);
                
                sem_post(MUTEX),sem_post(EMPTY);
            }
            exit(0);
        }
    }
    wait(NULL);

    sem_unlink("EMPTY");

    sem_unlink("FULL");

    sem_unlink("MUTEX");

    fclose(fp);

    return 0;
}
void num_in(FILE* fp,int num , int pos)
{

    fseek( fp, pos*sizeof(int) , SEEK_SET );
    fwrite( &num, 1, sizeof(num), fp);
    fflush(fp);
    return ;
}
void num_out(FILE* fp,int* num ,int pos)
{
    fseek( fp, pos*sizeof(int) , SEEK_SET );
    fread( num, sizeof(int),1, fp);
    return;
}
