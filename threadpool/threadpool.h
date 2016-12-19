#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <queue>
#include <vector>
#include <iostream>

using namespace std;

//单个任务
struct Job{
    void *(*callback_fun)(void*);  //任务处理的回调函数
    void *arg; //回调函数参数

    Job() : callback_fun(NULL), arg(NULL)
    {

    }

    Job(void *(*callback_fun)(void*), void *arg)
    {
        this->callback_fun = callback_fun;
        this->arg = arg;
    }
};

/**
 * @brief threadpool_fun  线程池中线程函数
 * @param arg  参数
 * @return
 */
void *threadpool_fun(void *arg);

//线程池类
class ThreadPool
{
public:
    ThreadPool();
    /**
     * @brief ThreadPool
     * @param thread_num  线程池中开启的线程个数
     * @param queue_max_count 任务队列中最大job的个数
     */
    ThreadPool(int thread_num, int queue_max_count = 20);

    ~ThreadPool();

    //设置为友元函数，以访问ThreadPool的私有数据
    friend void *threadpool_fun(void *arg);

    /**
     * @brief add_job    向任务队列添加任务
     * @param callback_fun  任务的回调函数
     * @param arg  回调函数的参数
     * @return  成功：0   失败：-1:任务无效；  -2：线程池已关闭
     */
    int add_job(void* (*callback_fun)(void *arg), void *arg);

    /**
     * @brief add_job   向任务队列添加任务
     * @param job  添加的任务
     * @return 成功：0   失败：-1:任务无效；  -2：线程池已关闭
     */
    int add_job(const struct Job &job);

    /**
     * @brief threadpool_destroy  销毁线程池
     * @return
     */
    int threadpool_destroy();

    int thread_num;  //线程池中开启的线程个数

    int queue_max_count; //任务队列中最大job的个数
    int queue_cur_count; //任务队列中当前job的个数，主要考虑到queue.size()效率太低

    /*设置两个标志位的原因：
     *当关闭线程池时，此时任务队列中可能存在未完成的任务，
     *这时要求不能向任务队列中添加任务，但应该执行线程，以完成队列中的任务
     */
    bool queue_close; //任务队列是否已经关闭
    bool pool_close; //线程池是否已经关闭

private:
    /**
     * @brief threadpool_init  线程池的初始化
     */
    void threadpool_init();

    vector<pthread_t> vec_pthreads; //线程池的pthread_t
    queue<Job> queue_job; //任务队列

    pthread_mutex_t mutex; //互斥信号量 用于任务队列的操作
    pthread_cond_t queue_empty; //队列为空的条件变量
    pthread_cond_t queue_not_empty;//队列不为空的条件变量
    pthread_cond_t queue_not_full;//队列不为满的条件变量

};

#endif // THREADPOOL_H
