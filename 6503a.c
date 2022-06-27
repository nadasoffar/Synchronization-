#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
int n;
int bsize;
time_t t;

#define TRUE 1
int messagecount;
sem_t countsemaphore; 
sem_t bufferaccesssemaphore; //allows only monitor or collector to acces buffer at a time
sem_t itemsinbuffer; //block consumer if buffer fady
sem_t numberofemptyspaces;//determines number of empty spaces in buffer & block producer if buffer is empty
int temp2;
int queue[400];
int rear;
int front;
int dequeuedvalue;
int collectorposition=0;
int monitorposition=0;


int IsEmpty(int arr[])
{
   if(front==-1 & rear==-1)
   	return 1;
   else 
   	return 0;
}


void enqueue(int arr[],int value)
{
	/*if((rear+1)%bsize==front)
	{
			printf("cannot insert in full queue");
	}*/
	if(IsEmpty(queue))
	{
		front=0;
		rear=0;
		queue[rear]=value;
	}
	else
	{
		rear=(rear+1)%bsize;
		queue[rear]=value;
	
	}
	
} 

void Dequeue(int arr[])
{

	/*if(IsEmpty)
	{
		printf("\nCan not delete from an empty queue\n");
	}*/
	if(front==rear) //one item in buffer
	{
		dequeuedvalue= queue[front];
		front=rear=-1;//queue will be empty after dequeue
		//return queue[front];
	}
	else 
	{
        dequeuedvalue=queue[front];
		front=(front+1)%bsize;

	}
	//return dequeuedvalue;
}



//sem_init(&countsemaphore, 0,1);


void *intvalue(void*threadnum)
{
	while(TRUE)
	{
		 sleep(rand()%3);
		int num= (int)threadnum;
	   printf("Counter thread %d: recieved a message\n",num);
	   printf("Counter thread %d waiting to write\n", num);
       sem_wait(&countsemaphore);
       //printf("Counter thread %d: now adding to counter, counter value:%d",num,messagecount);
       //printf("Counter thread %d recieved a message\n", num);
       messagecount++;
       printf("Counter thread %d: now adding to counter, counter value:%d\n",num,messagecount);
       sem_post(&countsemaphore);
       //sleep(rand());
       //sleep(c_rand);
       //sleep(rand()%3);
       //Thread.sleep(random.nextInt(1000));

	}
}

void *PlaceMessagecountInBuffer(void*threadnum) //producer
{
	while(TRUE)
	{
		sleep(rand()%4);
		int value2;
		sem_getvalue(&numberofemptyspaces,&value2);
		if(value2==0)
		{
			printf("Monitor thread:buffer full!");
		}

	/*if(monitorposition==bsize)
	{
		monitorposition=0;
	}	*/
    int temp;
   printf("\nMoniter thread: waiting to read counter\n ");
	sem_wait(&countsemaphore);
	temp=messagecount;
	printf("Moniter thread: reading a count value of:%d\n",temp);
    messagecount=0;
    sem_post(&countsemaphore);
	//int temp;
	//temp=messagecount;
	//printf("\nMoniter thread: waiting to read counter\n ");
	sem_wait(&numberofemptyspaces);
	sem_wait(&bufferaccesssemaphore);
	//sem_wait(&countsemaphore);
	//temp=messagecount;
	//printf("Moniter thread: reading a count value of:%d\n",temp);
	//temp=messagecount;
	//messagecount=0;
	if(monitorposition==bsize)
	{
		monitorposition=0;
	}	
	enqueue(queue,temp);
	monitorposition++;
	printf("Moniter thread: writing to buffer at position %d\n", monitorposition);
	//sem_post(&countsemaphore);
	sem_post(&bufferaccesssemaphore);
	sem_post(&itemsinbuffer);
	//sleep(rand());
    //Thread.sleep(random.nextInt(1000));
    //sleep(c_rand);        
    //sleep(rand()%3);
	}
}

void * collectmessages(void*(threadnum)) //consumer
{
	while(TRUE)
	{ 
		sleep(rand()%5);

		int value;
		sem_getvalue(&numberofemptyspaces,&value);
		if(value==bsize)
		{
			printf("\nCollecter thread: nothing is in the buffer!\n");
		}

		/*if(collectorposition==bsize)
		{
			collectorposition=0;
		}*/
		sem_wait(&itemsinbuffer);
		sem_wait(&bufferaccesssemaphore);
		if(collectorposition==bsize)
		{
			collectorposition=0;
		}
		Dequeue(queue);
		collectorposition++;
      printf("Collector thread: reading from buffer at position %d\n", collectorposition);
      sem_post(&bufferaccesssemaphore);
      sem_post(&numberofemptyspaces);

        
        //sleep(rand()%3);
		//sleep(rand());
		//Thread.sleep(random.nextInt(1000));
	}




}
void main()
{
	//int*queue=malloc(bsize*sizeof(int));
	srand((unsigned)time(&t));
	int front=rear=-1;
	messagecount=0;
	printf("Enter number of counter threads\n");
	scanf("%d", &n);
	printf("Enter size of buffer\n");
	scanf("%d", &bsize);
    /*int x =7;
	enqueue(queue,x);
	enqueue(queue,x);
	enqueue(queue,x);
	//enqueue(queue,x);
	
    for (int i=0;i<bsize;i++)
    {
    	printf("%d", queue[i]);
    }
    Dequeue(queue);
    Dequeue(queue);
    Dequeue(queue);
    Dequeue(queue);

    for (int i=0;i<bsize;i++)
    {
    	printf("%d", queue[i]);
    }*/
	sem_init(&countsemaphore, 0,1); 
	sem_init(&bufferaccesssemaphore,0,1);
    sem_init(&itemsinbuffer,0,0);
    sem_init(&numberofemptyspaces,0,bsize);
	pthread_t mcounter[n];
	for (int i=0;i<n;i++)
	{
	   pthread_create(&mcounter[i],NULL,intvalue,(void *)i);
	}

	pthread_t mMoniter;
	pthread_create(&mMoniter,NULL,PlaceMessagecountInBuffer,NULL );

	//pthread_join(mMonitor,NULL);
	pthread_t mCollector;
	pthread_create(&mCollector,NULL, collectmessages,NULL);

   // pthread_join(mCollector,NULL);
	for (int i=0;i<n;i++)
	{
	   pthread_join(mcounter[i],NULL);
	}

	pthread_join(mMoniter,NULL);
	pthread_join(mCollector,NULL);
    //sem_init(&countsemaphore, 0,1); 





}