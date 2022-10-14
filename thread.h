#ifndef __THREAD_H__
#define __THREAD_H__

#define MAX_THREAD_NUM 16 // 最大线程个数
#define STACK_SIZE 1024   // 每个线程栈大小

// 设置三种状态
#define THREAD_RUNNING 0 // 运行态
#define THREAD_READY 1   // 预备态
#define THREAD_EXIT 2    // 结束态

/// 创建线程结构体
struct threadStruct
{
    int id;                // 线程id
    void (*threadFunc)();  // 线程要执行的函数
    int esp;               // esp
    int status;            // 线程状态
    int stack[STACK_SIZE]; // 线程栈
    
};

int threadCreate(int *ID, void (*startFunc)());
int threadJoin(int ID);
void schedule();
unsigned int getmstime();

#endif
