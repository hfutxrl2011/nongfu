#include <Python.h>
#include "ThreadPool.h"
#include <stdio.h>
#include <assert.h>
#include "../util/log.h"

 ThreadPool::ThreadPool(int threadNum)
    {
        threadsNum_ = threadNum;
        isRunning_ = true;
    }

    void ThreadPool::start(){
        //isRunning_ = true;
		createThreads();
    }

    ThreadPool::~ThreadPool()
    {
        stop();
        for(std::deque<Task*>::iterator it = taskQueue_.begin(); it != taskQueue_.end(); ++it)
        {
            delete *it;
        }
        taskQueue_.clear();
		//PyGILState_Ensure();
		//Py_Finalize();
    }

    int ThreadPool::createThreads()
    {
        pthread_mutex_init(&mutex_, NULL);
        pthread_cond_init(&condition_, NULL);
        threads_ = (pthread_t*)malloc(sizeof(pthread_t) * threadsNum_);
		
		//if ( !Py_IsInitialized() ){Py_Initialize();}
		//PyEval_InitThreads();
		//PyEval_ReleaseThread(PyThreadState_Get());
		
		//PyThreadState  *  mainThreadState  =  NULL;
		 //  save a pointer to the main PyThreadState object 
		 //mainThreadState  =  PyThreadState_Get();
		 //  release the lock 
		 //PyEval_ReleaseLock();
		
        for (int i = 0; i < threadsNum_; i++)
        {
            int ret = pthread_create(&threads_[i], NULL, threadFunc, this);
			log_info("thread i:%lu,ret:%d",threads_[i],ret);
        }
		
		
		
        return 0;
    }

    size_t ThreadPool::addTask(Task *task)
    {
        pthread_mutex_lock(&mutex_);
		int size = taskQueue_.size();
		if(size <= 10){
			taskQueue_.push_back(task);
		}else{
			log_info("debug...the queue size is over 10,size:%d",size);
		}
        size = taskQueue_.size();
        pthread_mutex_unlock(&mutex_);
        pthread_cond_signal(&condition_);
        return size;
    }

    void ThreadPool::stop()
    {
        if (!isRunning_)
        {
            return;
        }

        isRunning_ = false;
        pthread_cond_broadcast(&condition_);

        for (int i = 0; i < threadsNum_; i++)
        {
            pthread_join(threads_[i], NULL);
        }

        free(threads_);
        threads_ = NULL;

        pthread_mutex_destroy(&mutex_);
        pthread_cond_destroy(&condition_);
    }

    int ThreadPool::size()
    {
        pthread_mutex_lock(&mutex_);
        int size = taskQueue_.size();
        pthread_mutex_unlock(&mutex_);
        return size;
    }

    Task* ThreadPool::take()
    {
        Task* task = NULL;
        while (!task)
        {
            pthread_mutex_lock(&mutex_);
            while (taskQueue_.empty() && isRunning_)
            {
                pthread_cond_wait(&condition_, &mutex_);
            }

            if (!isRunning_)
            {
                pthread_mutex_unlock(&mutex_);

                break;
            }
            else if (taskQueue_.empty())
            {
                pthread_mutex_unlock(&mutex_);
                continue;
            }
			log_info("debug... queue size:%d",taskQueue_.size());
            assert(!taskQueue_.empty());
            task = taskQueue_.front();
            taskQueue_.pop_front();
			log_info("debug... queue size:%d",taskQueue_.size());
            pthread_mutex_unlock(&mutex_);
        }
        return task;
    }

    void* ThreadPool::threadFunc(void* arg)
    {
        pthread_t tid = pthread_self();
		
        ThreadPool* pool = static_cast<ThreadPool*>(arg);
		log_info("debug 1 ....");
        while (pool->isRunning_)
        {
            Task* task = pool->take();
			log_info("debug... thread take the task tid:%lu",tid);
            if (!task)
            {
                log_info("thread %lu will exit\n", tid);
                continue;
				//break;
            }
			log_info("debug 2 ....");
            assert(task);
            task->run();
        }
		log_info("debug 3 ....");
        return 0;
    }
    //bool ThreadPool::getisRunning_(){return isRunning_;}