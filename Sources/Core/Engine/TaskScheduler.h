#pragma once
#include <ThreadPool.h>
#include <Common\Constants.h>

#define GTaskScheduler Engine::g_TaskScheduler
#define ScheduleTask(F, ...) g_TaskScheduler.Schedule(F, __VA_ARGS__)
#define TASK(callable) [=]() { (callable)(); }

namespace Engine
{
    class TaskScheduler
    {
    public:
        void Init();
        void Destroy();

#ifdef SINGLE_THREAD
        void Execute();
#endif

        template<typename F, typename ...Args>
        void Schedule(F&& f, Args&& ...args)
        {
#ifdef SINGLE_THREAD
            auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
            mTask.push(task);
#else
            mThreadPool->enqueue(std::forward<F>(f), std::forward<Args>(args)...);
#endif
        }

    private:
#ifndef SINGLE_THREAD
        ThreadPool* mThreadPool;
#else
        std::queue<std::function<void()>> mTask;
#endif
    };

    extern TaskScheduler g_TaskScheduler;
}