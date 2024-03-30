#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <pthread.h>
#include <semaphore.h>
#define NR_THREADS 5

typedef struct
{
    int id;     //valoarea thread-ului
    int proces; //procesul din care face parte thread-ul
    
}TH_STRUCT;    

pthread_mutex_t lock1;
pthread_mutex_t lock2;

sem_t max;
sem_t thread2_14;

void* bariere2(void* arg)
{
    TH_STRUCT* t = (TH_STRUCT*)arg;
    if(t->id == 14) //T2.14
    {
    	for(int i=0; i<6; i++)
    	{
    	    sem_wait(&max);
    	}
    }
    else
    {
    	sem_wait(&max);
    }
    info(BEGIN, 2, t->id);
    info(END, 2, t->id);
    sem_post(&max);
    if(t->id == 14)
    {
    	sem_post(&thread2_14);
    }
    
    pthread_exit(NULL);
    return NULL;
}

void* bariere6(void* arg)
{
    TH_STRUCT* t = (TH_STRUCT*)arg;
    info(BEGIN, 6, t->id);
    info(END, 6, t->id);
    return NULL;
}

void* sincronizare(void* arg)
{
    ///T3.5 sa inceapa inainte de T3.3 si sa se incheie dupa terminarea acestuia
    ///am folosit lab 9.2.6.5
    TH_STRUCT *t = (TH_STRUCT*)arg;
    if (t->proces == 3 && t->id == 3)
	pthread_mutex_lock(&lock1);
	
    info(BEGIN, 3, t->id);
    
    if (t->proces == 3 && t->id == 5)
	pthread_mutex_unlock(&lock1);
	
	
    if (t->proces == 3 && t->id == 5)
	pthread_mutex_lock(&lock2);
	
    info(END, 3, t->id);
    
    if (t->proces == 3 && t->id == 3)
	pthread_mutex_unlock(&lock2);
    return NULL;
}

void ierarhie_procese()
{
    pid_t p5 = -1, p2 = -1, p3 = -1, p4 = -1;
    pid_t p6 = -1, p7 = -1, p8 = -1, p9 = -1;

    p2 = fork();
    if(p2 == 0)
    {   
    	info(BEGIN, 2, 0);
    	
        p6 = fork();
        if(p6 == 0)
        {
            info(BEGIN, 6, 0);
            pthread_t tids[5];
            TH_STRUCT params[5] = {{1,6}, {2,6}, {3,6}, {4,6}, {5,6}};
            
            for(int i=0; i<5; i++)
            {
                pthread_create(&tids[i], NULL, bariere6, &params[i]);
            }
            for(int i=0; i<5; i++)
            {
                pthread_join(tids[i], NULL);
            }
            info(END, 6, 0);
            exit(0);
        }
        wait(NULL); //P6
        
        p7 = fork();
        if(p7 == 0)
        {
            info(BEGIN, 7, 0);
            info(END, 7, 0);
            exit(0);
        }
        wait(NULL); //P7
        
        pthread_t tids[43];
        TH_STRUCT params[43];
        sem_init(&max, 0, 6);
        sem_init(&thread2_14, 0, 0);
        for(int i=0; i<43; i++)
        {
            params[i].id = i+1;
            params[i].proces = 2;
            pthread_create(&tids[i], NULL, bariere2, &params[i]);
        }
        sem_wait(&thread2_14);
        for(int i=0; i<43; i++)
        {
            pthread_join(tids[i], NULL);
        }
        sem_destroy(&max);
        sem_destroy(&thread2_14);
        
        info(END, 2, 0);
        exit(0);
    }
    wait(NULL); //P2
    
    p3 = fork();
    if(p3 == 0) 
    {
       info(BEGIN, 3, 0);
       
       if(pthread_mutex_init(&lock1, NULL) != 0 || pthread_mutex_init(&lock2, NULL))
       {
           perror("error initializing the mutex");
           return;
       }
			
       pthread_t tids[NR_THREADS];
       pthread_mutex_lock(&lock1);
       pthread_mutex_lock(&lock2);
       TH_STRUCT params[NR_THREADS] = {{1,3}, {2,3}, {3,3}, {4,3}, {5,3}}; ///pentru T3.1 T3.2 T3.3 T3.4 T3.5 din P3

       for(int i=0; i<NR_THREADS; i++)
       {
           pthread_create(&tids[i], NULL, sincronizare, &params[i]);
       }
       for(int i=0; i<NR_THREADS; i++)
       {
           pthread_join(tids[i], NULL);
       }
       
       pthread_mutex_destroy(&lock1);
       pthread_mutex_destroy(&lock2);
        
       info(END, 3, 0);
       exit(0);
    }
    wait(NULL); //P3
    
    p4 = fork();
    if(p4 == 0)
    { 
        info(BEGIN, 4, 0);
        p9 = fork();
        if(p9 == 0)
        {
            info(BEGIN, 9, 0);
            info(END, 9, 0);
            exit(0);
        }
        wait(NULL); //P9
        info(END, 4, 0);
        exit(0);
    }
    wait(NULL); //P4
    
    p5 = fork();
    if(p5 == 0) 
    {
        info(BEGIN, 5, 0);
        info(END, 5, 0);
        exit(0);
    }
    wait(NULL); //P5
    
    p8 = fork();
    if(p8 == 0) 
    {
        info(BEGIN, 8, 0);
        info(END, 8, 0);
        exit(0);
    }
    wait(NULL); //P8
}

int main()
{
    init();
    info(BEGIN, 1, 0);
    ierarhie_procese();
    info(END, 1, 0);
    return 0;
}
