#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "thread.h"

void fun1()
{
    int i = 6;
    struct timeval start;
    struct timeval end;
    double totalTime;
    while (i--)
    {
        // 记录每次函数运行的时间，累加超过时间片则切换调度
        gettimeofday(&start, NULL);
        printf("fun1 is running\n");
        usleep(1000 * 500); // 每次运行完停止一段时间，方便测试
        gettimeofday(&end, NULL);
        totalTime += 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000;
        if (totalTime > 1000)
        {
            totalTime = 0;
            schedule();
        }
    }
}

void fun2()
{
    int i = 4;
    struct timeval start;
    struct timeval end;
    double totalTime;
    while (i--)
    {
        gettimeofday(&start, NULL);
        printf("fun2 is running\n");
        usleep(1000 * 300);
        gettimeofday(&end, NULL);
        totalTime += 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000;
        if (totalTime > 1000)
        {
            totalTime = 0;
            schedule();
        }
    }
}

void fun3()
{
    int i = 4;
    struct timeval start;
    struct timeval end;
    double totalTime;
    while (i--)
    {
        gettimeofday(&start, NULL);
        printf("fun3 is running\n");
        usleep(1000 * 300);
        gettimeofday(&end, NULL);
        totalTime += 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000;
        if (totalTime > 1000)
        {
            totalTime = 0;
            schedule();
        }
    }
}

int main()
{
    int ID1, ID2, ID3;
    threadCreate(&ID1, fun1);
    printf("创建线程%d\n", ID1);
    threadCreate(&ID2, fun2);
    printf("创建线程%d\n", ID2);
    threadCreate(&ID3, fun3);
    printf("创建线程%d\n", ID3);

    threadJoin(ID1);
    threadJoin(ID2);
    threadJoin(ID3);

    return 0;
}
