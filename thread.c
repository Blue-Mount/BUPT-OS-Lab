#include "thread.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h> // 用于时间片计时

static struct threadStruct initThread = {0, NULL, 0, THREAD_READY, {0}}; // 初始化空线程
struct threadStruct *current = &initThread;                              // 初始化current线程

// 初始化线程列表
struct threadStruct *threadList[MAX_THREAD_NUM] = {
    &initThread,
};

void switchTo(struct threadStruct *next);
void schedule();

// 线程启动
static void start(struct threadStruct *tsk)
{
    tsk->status = THREAD_RUNNING; // 设置成RUNNING态
    tsk->threadFunc();            // 开始执行函数
    tsk->status = THREAD_EXIT;    // 函数结束后切换成EXIT态
    printf("线程%d结束\n\n", tsk->id);

    schedule(); // 结束后直接调用下一个线程，不等时间片结束
}

int threadCreate(int *ID, void (*start_routine)())
{
    int id = -1;
    struct threadStruct *tsk = (struct threadStruct *)malloc(sizeof(struct threadStruct));

    // 在线程列表中寻找空位置给新线程
    while (++id < MAX_THREAD_NUM && threadList[id])
        ;

    // 如果没找到，则无法新建线程
    if (id == MAX_THREAD_NUM)
    {
        return -1;
    }

    // 找到了则开始赋值
    threadList[id] = tsk;
    *ID = id;

    tsk->id = id;
    tsk->threadFunc = start_routine;
    int *stack = tsk->stack;                   //设置栈顶
    tsk->esp = (int)(stack + STACK_SIZE - 11); // 设置esp
    tsk->status = THREAD_READY;

    stack[STACK_SIZE - 3] = (int)start; // 压入线程要执行的函数
    stack[STACK_SIZE - 2] = 100;        // 如果 start 执行结束，表明线程结束
    stack[STACK_SIZE - 1] = (int)tsk;   // 要执行函数的参数

    return 0;
}

int threadJoin(int ID)
{
    printf("\n线程%d调用join函数\n", ID);

    //如果还没运行完就走到了join，则继续执行，不能直接free空间
    while (threadList[ID]->status != THREAD_EXIT)
    {
        schedule();
    }
    free(threadList[ID]);  // 执行完毕，已经是EXIT态，按照要求释放栈空间
    threadList[ID] = NULL; // 腾出列表给新线程，以免新线程一直不能创建
}

//该函数用于获取当前时间，ms单位
unsigned int getmstime()
{
    struct timeval t;
    if (gettimeofday(&t, NULL) < 0)
    {
        exit(-1);
    }
    return t.tv_sec * 1000 + t.tv_usec / 1000;
}

// 该函数用于选择下一个调度哪个线程，让代码整体可以执行更好，以后修改调度算法更加方便
static struct threadStruct *pickNext()
{
    struct threadStruct *next = NULL; // 创建next存放准备调度的线程
    int current_id = current->id;     // 记录前一个现成的id
    int i = current_id;               // 创建迭代器i

    while (1)
    {
        i = (i + 1) % MAX_THREAD_NUM;

        // 轮换制，直接选择下一个
        if (threadList[i] && threadList[i]->status == THREAD_READY)
        {
            printf("\n切换到线程%d\n", i);
            next = threadList[i];
            break;
        }
    }

    return next;
}

// 最核心的调度函数
void schedule()
{
    struct threadStruct *next = pickNext();
    if (next)
    {
        // 避免已经切换成EXIT态的线程又重新被设置成READY，从而导致段错误的情况，此处设置一个判断
        if (current->status != THREAD_EXIT)
        {
            printf("切换线程%d状态到READY\n", current->id);
            current->status = THREAD_READY;
        }
        printf("切换线程%d状态到RUNNING\n", next->id); //下一个线程开始运行，切换成RUNNING态
        next->status = THREAD_RUNNING;
        switchTo(next); // 汇编语言见 switch.s 文件
    }
}
