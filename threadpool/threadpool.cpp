#include "threadpool.h"

ThreadPool::ThreadPool()
{
    thread_num = 10;
    queue_max_count = 20;

    queue_cur_count = 0;
    queue_close = false;
    pool_close = false;

    threadpool_init();
}

ThreadPool::ThreadPool(int thread_num, int queue_max_count)
    :thread_num(thread_num), queue_max_count(queue_max_count)
{
    queue_cur_count = 0;
    queue_close = false;
    pool_close = false;

    threadpool_init();
}

ThreadPool::~ThreadPool()
{
    threadpool_destroy();
}

void ThreadPool::threadpool_init()
{
    //初始化互斥变量以及条件条件变量
    if(pthread_mutex_init(&mutex, NULL) != 0)
    {
        perror("pthread_mutex_init");
        exit(-1);
    }
    if(pthread_cond_init(&queue_empty, NULL) != 0)
    {
        perror("pthread_cond_init");
        exit(-1);
    }
    if(pthread_cond_init(&queue_not_empty, NULL) != 0)
    {
        perror("pthread_cond_init");
        exit(-1);
    }
    if(pthread_cond_init(&queue_not_full, NULL) != 0)
    {
        perror("pthread_cond_init");
        exit(-1);
    }

    //初始化线程池
    for(int i = 0; i < thread_num; i ++)
    {
        pthread_t tid;
        if(pthread_create(&tid, NULL, threadpool_fun, this) != 0)
        {
            perror("pthread_create");
            exit(-1);
        }
        vec_pthreads.push_back(tid);
    }
}

int ThreadPool::add_job(void *(*callback_fun)(void *), void *arg)
{
    Job job(callback_fun, arg);
    return add_job(job);
}

int ThreadPool::add_job(const Job &job)
{
    //判断job是否有效，若无效，则直接返回-1
    if(NULL == job.callback_fun)
        return -1;

    pthread_mutex_lock(&mutex);   
    /*如果满足如下所有情况，则进行阻塞
     *任务队列满
     *任务队列未关闭
     *线程池未关闭
     */
    while(queue_cur_count == queue_max_count && !(queue_close || pool_close))
    {
        pthread_cond_wait(&queue_not_full, &mutex);
    }
    if(queue_close || pool_close)
    {
        //线程池已关闭
        pthread_mutex_unlock(&mutex);
        return -2;
    }
    queue_job.push(job);
    queue_cur_count ++;
    //1时，说明先前任务队列为空
    if(1 == queue_cur_count)
    {
        //唤醒睡眠的线程
        pthread_cond_broadcast(&queue_not_empty);
    }
    pthread_mutex_unlock(&mutex);

    return 0;
}

int ThreadPool::threadpool_destroy()
{
    pthread_mutex_lock(&mutex);

    if(queue_close || pool_close)
    {
        //说明已经关闭了,返回-1
        pthread_mutex_unlock(&mutex);
        return -1;
    }
    queue_close = true;
    //只有当任务队列为空时，才进行彻底销毁线程池
    while(queue_cur_count != 0)
    {
        pthread_cond_wait(&queue_empty, &mutex);
    }
    pool_close = true;
    pthread_mutex_unlock(&mutex);
    pthread_cond_broadcast(&queue_not_empty);
    pthread_cond_broadcast(&queue_not_full);

    for(int i = 0; i < thread_num; i ++)
    {
        pthread_join(vec_pthreads[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&queue_empty);
    pthread_cond_destroy(&queue_not_empty);
    pthread_cond_destroy(&queue_not_full);

    return 0;
}


void *threadpool_fun(void *arg)
{
    //设置线程分离，如果线程退出，则释放线程资源
//    pthread_detach(pthread_self());

    ThreadPool *pool = (ThreadPool*) arg;

    while(1)
    {
        pthread_mutex_lock(&pool->mutex);
        /*如果满足如下所有情况，则进行阻塞
         *任务队列为空
         *线程池未关闭
         */
        while((0 == pool->queue_cur_count) && !pool->pool_close)
        {
//            printf("queue_cur_count: %d\n", pool->queue_cur_count);
            pthread_cond_wait(&pool->queue_not_empty, &pool->mutex);
        }
//        printf("woke\n");
        if(pool->pool_close)
        {
            pthread_mutex_unlock(&pool->mutex);
            pthread_exit(NULL);
        }
        Job job = pool->queue_job.front();
        pool->queue_job.pop();
        pool->queue_cur_count --;
        //判断任务队列是否为空，如果要销毁线程池，唤醒pool->threadpool_destroy()
        if(0 == pool->queue_cur_count)
        {
            pthread_cond_signal(&pool->queue_empty);
        }
        //判断原先任务队列是否已满,若曾经已满，则唤醒pool->add_job()
        if(pool->queue_cur_count == pool->queue_max_count - 1)
        {
            pthread_cond_broadcast(&pool->queue_not_full);
        }
        pthread_mutex_unlock(&pool->mutex);
        //执行任务
        job.callback_fun(job.arg);
    }

}
