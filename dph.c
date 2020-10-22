#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define PHILOSOPHER_NUMBERS 5
#define MAX_SLEEP_TIME	3
//最大休眠时间
enum {THINKING, HUNGRY, EATING} state[PHILOSOPHER_NUMBERS];
//定义哲学家的三种基本状态，分别是THINKING,HUNGRY和EATING。

pthread_cond_t self[PHILOSOPHER_NUMBERS];// Behalf on each philosophers
pthread_mutex_t mutex[PHILOSOPHER_NUMBERS]; // For conditional variables
//使用条件变量来使得哲学家们在还不满足吃饭条件的时候进行等待，
//并给每一个哲学家的条件变量配上一把互斥锁。

pthread_t tid[PHILOSOPHER_NUMBERS];// For pthread_create()
int id[PHILOSOPHER_NUMBERS];// For philo function
//定义五个线程的id，以及给线程函数传递的线程索引数组

//part5
void (*ptr)(int);
void func(int signum){
    printf("\nKilling the philosophers...\n");
    for (int i=0;i<5;i++){
        pthread_cancel(tid[i]);
    }
    for (int i=0;i<5;i++){
        pthread_cond_destroy(&self[i]);
        pthread_mutex_destroy(&mutex[i]);
    }
    printf("Killed philosophers.\n");
}

//part4
void test(int i){
    // A philosopher can eat when he wants to eat and his neighbors aren't eating.
    if ( (state[(i+PHILOSOPHER_NUMBERS-1)%PHILOSOPHER_NUMBERS] != EATING)&&
    (state[i] == HUNGRY) &&
    (state[(i+1)%PHILOSOPHER_NUMBERS] != EATING)
    ){
        pthread_mutex_lock(&mutex[i]);
        state[i] = EATING;
        pthread_cond_signal(&self[i]);
        pthread_mutex_unlock(&mutex[i]);
    }
}//判断我是否能进入吃饭状态，如果能，进入吃饭状态并解除挂起状态。

//part3
void pickup_forks(int i){
    state[i] = HUNGRY;// Wants to eat
    test(i);// Check can eat or not
    pthread_mutex_lock(&mutex[i]);
    while (state[i] != EATING){
        pthread_cond_wait(&self[i],&mutex[i]);//Wait his neighbors ate
    }
    pthread_mutex_unlock(&mutex[i]);
}//模拟拿起筷子
//函数接受一个参数i，代表哲学家的索引。
//拿起筷子前首先要把哲学家的状态设为饥饿（这样他才有机会拿起筷子）
//然后调用test函数判断是否满足吃饭条件（判断他的邻居们是否都不在吃饭）。
//如果不满足上述条件，该哲学家（基于条件变量）将会被挂起（相当于他在等待有饭吃）
//等待别人通知他可以吃饭（基于条件变量的唤醒）。

//part2
void return_forks(int i){
    state[i] = THINKING;
    //Notify his neighbor that I was ate.
    test((i+PHILOSOPHER_NUMBERS-1)%PHILOSOPHER_NUMBERS);
    test((i+1)%PHILOSOPHER_NUMBERS);
}//模拟放下筷子
//哲学家吃完之后会陷入思考阶段。并通知他的邻居们“我吃完饭了”，
//test函数的逻辑为：如果我的邻居们都没在吃饭，并且我饿了，
//我就进入吃饭状态，并解除我的挂起状态（如果我饿了，并且我未能切到吃饭状态前那我一定是被挂起的）。

//part1:
#define TRUE 1
void *philo(void *param){
    do{
    int id = *( (int *)param);
    /* Try to pickup a chopstick*/
    pickup_forks(id);
    printf("The philosopher %d is eating...\n",id);
    /* Eat a while*/
    srand((unsigned)time(NULL));
    int sec = (rand()%((3-1)+1)) +1;// make sec in [1,3]
    sleep(sec);
    /* Return a chopstick */
    return_forks(id);
    printf("The philosopher %d is thinking...\n",id);
    /* Think a while */
    srand((unsigned)time(NULL));
    sec = (rand()%((3-1)+1)) +1;// make sec in [1,3]
    sleep(sec);
    }while(TRUE);
    pthread_exit(NULL);
}
//传递一个索引给线程函数，使其知道自己是第几号哲学家。
//然后哲学家试图拿起两只筷子，如果成功拿起两只筷子，
//就随机睡眠一段时间（模拟吃饭）。
//然后放下筷子，并随机睡眠一段时间（模拟思考），重复上述过程。



int main()
{
	//signal(SIGINT,func);	
	int i;
	for (i = 0; i < PHILOSOPHER_NUMBERS; i++)
		pthread_join(tid[i],NULL);
	return 0;
}
