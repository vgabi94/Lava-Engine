#include "TaskScheduler.h"

namespace Engine
{
    TaskScheduler g_TaskScheduler;

    void TaskScheduler::Init()
    {
#ifndef SINGLE_THREAD
        mThreadPool = new ThreadPool(MAX_TASKS_PER_FRAME);
        LOG_INFO("[LOG] Thread pool create.\n");
#endif
    }

    void TaskScheduler::Destroy()
    {
#ifndef SINGLE_THREAD
        delete mThreadPool;
        LOG_INFO("[LOG] Thread pool delete.\n")
#endif
    }

#ifdef SINGLE_THREAD
    void TaskScheduler::Execute()
    {
        for (size_t i = 0; i < MAX_TASKS_PER_FRAME && !mTask.empty(); i++)
        {
            mTask.front()();
            mTask.pop();
        }
    }
#endif
}