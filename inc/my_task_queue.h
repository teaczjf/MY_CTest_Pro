#ifndef _MY_TASK_QUEUE_H_
#define _MY_TASK_QUEUE_H_
#include <atomic>
#include <queue>
#include <memory>
#include <mutex>
#include <thread>
#include <iostream>
#include <functional>

using namespace std;

//任务接口
class WorkItem
{
public:
    //接口方法必须在子类实现
    virtual void run() = 0;

public:
    //任务清理接口
    virtual void clean()
    {
    }
    //判断任务是否可执行(返回真时任务才会执行)
    virtual bool runnable()
    {
        return true;
    }
};

//自旋锁类
class SpinMutex
{
private:
    atomic_flag flag = ATOMIC_FLAG_INIT;

public:
    void lock()
    {
        while (flag.test_and_set(memory_order_acquire))
            ;
    }
    void unlock()
    {
        flag.clear(memory_order_release);
    }
};

//任务队列
class AsyncExecQueue
{
public:
    size_t maxsz;   //最大任务数
    size_t threads; //最大线程数
    mutable SpinMutex mtx;
    std::queue<shared_ptr<WorkItem>> que;

    AsyncExecQueue()
    {
        this->maxsz = 0;
    }

    bool pop(shared_ptr<WorkItem> &item)
    {
        std::lock_guard<SpinMutex> lk(mtx);
        if (que.empty())
            return false;
        item = que.front();
        que.pop();
        return true;
    }

public:
    //实现单例模式
    // static AsyncExecQueue *Instance()
    // {
    //     static AsyncExecQueue obj;
    //     return &obj;
    // }

public:
    //中止任务处理
    void stop()
    {
        threads = 0;
    }
    //清空队列
    void clear()
    {
        std::lock_guard<SpinMutex> lk(mtx);
        while (que.size() > 0)
            que.pop();
    }
    //判断队列是否为空
    bool empty() const
    {
        std::lock_guard<SpinMutex> lk(mtx);
        return que.empty();
    }
    //获取队列深度
    size_t size() const
    {
        std::lock_guard<SpinMutex> lk(mtx);
        return que.size();
    }
    //获取任务线程线
    size_t getThreads() const
    {
        return threads;
    }
    //任务对象入队
    bool push(shared_ptr<WorkItem> item)
    {
        std::lock_guard<SpinMutex> lk(mtx);
        if (maxsz > 0 && que.size() >= maxsz)
            return false;
        que.push(item);
        return true;
    }
    //启动任务队列(启动处理线程)
    void start(size_t threads = 4, size_t maxsz = 10000)
    {
        this->threads = threads;
        this->maxsz = maxsz;

        for (size_t i = 0; i < threads; i++)
        {
            std::thread(std::bind(&AsyncExecQueue::run, this)).detach();
        }
    }

public:
    //这个方法里面处理具体任务
    void run()
    {
        shared_ptr<WorkItem> item;

        while (threads > 0)
        {
            if (pop(item))
            {
                if (item->runnable())
                {
                    item->run();
                    item->clean();
                }
                else
                {
                    std::lock_guard<SpinMutex> lk(mtx);
                    que.push(item);
                }
            }
            else
            {
                std::chrono::milliseconds dura(1);
                std::this_thread::sleep_for(dura);
            }
        }
    }
};

//实现一个任务接口
class Task : public WorkItem
{
    string name;

public:
    //这个方法里面实现具体任务
    void run()
    {
        cout << "异步处理任务[" << name << "]..." << endl;
    }

public:
    Task(const string &name)
    {
        this->name = name;
    }
};

#endif
