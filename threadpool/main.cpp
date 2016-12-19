#include <iostream>
#include "threadpool.h"
using namespace std;

void* work(void* arg)
{
    int value = (int)(arg);
    printf("threadpool callback fuction : %d.\n", value);
    sleep(1);
}

int main()
{
    ThreadPool pool;
    pool.add_job(work, (void *)1);
    pool.add_job(work, (void *)2);
    pool.add_job(work, (void *)3);
    pool.add_job(work, (void *)4);
    pool.add_job(work, (void *)5);
    pool.add_job(work, (void *)6);
    pool.add_job(work, (void *)7);
    pool.add_job(work, (void *)8);
    pool.add_job(work, (void *)9);
    pool.add_job(work, (void *)10);
    pool.add_job(work, (void *)11);
    pool.add_job(work, (void *)12);
    pool.add_job(work, (void *)13);
    pool.add_job(work, (void *)14);
    pool.add_job(work, (void *)15);
    pool.add_job(work, (void *)16);
    pool.add_job(work, (void *)17);
    pool.add_job(work, (void *)18);
    pool.add_job(work, (void *)19);
    pool.add_job(work, (void *)20);
    pool.add_job(work, (void *)21);
    pool.add_job(work, (void *)22);
    pool.add_job(work, (void *)23);
    pool.add_job(work, (void *)24);
    pool.add_job(work, (void *)25);
    pool.add_job(work, (void *)26);
    pool.add_job(work, (void *)27);
    pool.add_job(work, (void *)28);
    pool.add_job(work, (void *)29);
    pool.add_job(work, (void *)30);
    pool.add_job(work, (void *)31);
    pool.add_job(work, (void *)32);
    pool.add_job(work, (void *)33);
    pool.add_job(work, (void *)34);
    pool.add_job(work, (void *)35);
    pool.add_job(work, (void *)36);
    pool.add_job(work, (void *)37);
    pool.add_job(work, (void *)38);
    pool.add_job(work, (void *)39);
    pool.add_job(work, (void *)40);

    sleep(5);
    return 0;
}

