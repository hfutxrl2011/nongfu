 
#ifndef UTIL_THREAD_H_
#define UTIL_THREAD_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <queue>
#include <vector>
#include <sys/time.h>
#include "exception.h"
#include "thread_mem.h"
#include "tools.h"

class Mutex{
	private:
		pthread_mutex_t mutex;
	public:
		Mutex(){
			pthread_mutex_init(&mutex, NULL);
		}
		~Mutex(){
			pthread_mutex_destroy(&mutex);
		}
		void lock(){
			pthread_mutex_lock(&mutex);
		}
		void unlock(){
			pthread_mutex_unlock(&mutex);
		}
};

class Locking{
	private:
		Mutex *mutex;
		Locking(const Locking&);
		void operator=(const Locking&);
	public:
		Locking(Mutex *mutex){
			this->mutex = mutex;
			this->mutex->lock();
		}
		~Locking(){
			this->mutex->unlock();
		}

};

template <class T>
class Queue{
	private:
		pthread_cond_t cond;
		pthread_mutex_t mutex;
		std::queue<T> items;
	public:
		Queue();
		~Queue();

		bool empty();
		int size();
		int push(const T item);
		int pop(T *data);
};

template <class T>
class SelectableQueue{
	private:
		int fds[2];
		pthread_mutex_t mutex;
		std::queue<T> items;
	public:
		SelectableQueue();
		~SelectableQueue();
		int fd(){
			return fds[0];
		}
		int size();
		// multi writer
		int push(const T item);
		// single reader
		int pop(T *data);
};

template<class W, class JOB>
class WorkerPool{
	public:
		class Worker{
			public:
				Worker(){};
				Worker(const std::string &name);
				virtual ~Worker(){}
				int id;
				virtual void init(){}
				virtual void destroy(){}
				virtual int proc(JOB job) = 0;
			private:
			protected:
				std::string name;
		};
	private:
		std::string name;
		Queue<JOB> jobs;
		SelectableQueue<JOB> results;

		int num_workers;
		std::vector<pthread_t> tids;
		bool started;
		volatile bool should_stop;

		struct run_arg{
			int id;
			WorkerPool *tp;
		};
		static void* _run_worker(void *arg);
	public:
		WorkerPool(const char *name="");
		~WorkerPool();

		int fd(){
			return results.fd();
		}
		
		int start(int num_workers);
		int stop();
		
		int push(JOB job);
		int pop(JOB *job);
};





template <class T>
Queue<T>::Queue(){
	pthread_cond_init(&cond, NULL);
	pthread_mutex_init(&mutex, NULL);
}

template <class T>
Queue<T>::~Queue(){
	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);
}

template <class T>
bool Queue<T>::empty(){
	bool ret = false;
	if(pthread_mutex_lock(&mutex) != 0){
		return -1;
	}
	ret = items.empty();
	pthread_mutex_unlock(&mutex);
	return ret;
}

template <class T>
int Queue<T>::size(){
	int ret = -1;
	if(pthread_mutex_lock(&mutex) != 0){
		return -1;
	}
	ret = items.size();
	pthread_mutex_unlock(&mutex);
	return ret;
}

template <class T>
int Queue<T>::push(const T item){
	if(pthread_mutex_lock(&mutex) != 0){
		return -1;
	}
	{
		items.push(item);
	}
	pthread_mutex_unlock(&mutex);
	pthread_cond_signal(&cond);
	return 1;
}

template <class T>
int Queue<T>::pop(T *data){
	if(pthread_mutex_lock(&mutex) != 0){
		return -1;
	}
	{
		while(items.empty()){
			struct timespec ts;
			struct timeval tv;
			gettimeofday(&tv, NULL);
			ts.tv_sec = tv.tv_sec;
			int32_t usec = tv.tv_usec + 50 * 1000; //50ms
			if(usec >= 1000000){
				ts.tv_sec += 1;
				usec -= 1000000;
			}
			ts.tv_nsec = usec * 1000;
			int wait_ret = 0;
			if((wait_ret = pthread_cond_timedwait(&cond, &mutex, &ts)) != 0){
				if(pthread_mutex_unlock(&mutex) != 0){
					return -1;
				}
				if(ETIMEDOUT != wait_ret){
					return -1;
				}
				return 0; //need to continue
			}
		}
		*data = items.front();
		items.pop();
	}
	if(pthread_mutex_unlock(&mutex) != 0){
		return -1;
	}
	return 1;
}


template <class T>
SelectableQueue<T>::SelectableQueue(){
	if(pipe(fds) == -1){
		fprintf(stderr, "create pipe error\n");
		exit(0);
	}
	pthread_mutex_init(&mutex, NULL);
}

template <class T>
SelectableQueue<T>::~SelectableQueue(){
	pthread_mutex_destroy(&mutex);
	close(fds[0]);
	close(fds[1]);
}

template <class T>
int SelectableQueue<T>::push(const T item){
	if(pthread_mutex_lock(&mutex) != 0){
		return -1;
	}
	{
		items.push(item);
	}
	if(::write(fds[1], "1", 1) == -1){
		fprintf(stderr, "write fds error\n");
		exit(0);
	}
	pthread_mutex_unlock(&mutex);
	return 1;
}

template <class T>
int SelectableQueue<T>::size(){
	int ret = 0;
	pthread_mutex_lock(&mutex);
	ret = items.size();
	pthread_mutex_unlock(&mutex);
	return ret;
}

template <class T>
int SelectableQueue<T>::pop(T *data){
	int n, ret = 1;
	char buf[1];

	while(1){
		n = ::read(fds[0], buf, 1);
		if(n < 0){
			if(errno == EINTR){
				continue;
			}else{
				return -1;
			}
		}else if(n == 0){
			ret = -1;
		}else{
			if(pthread_mutex_lock(&mutex) != 0){
				return -1;
			}
			{
				if(items.empty()){
					fprintf(stderr, "%s %d error!\n", __FILE__, __LINE__);
					pthread_mutex_unlock(&mutex);
					return -1;
				}
				*data = items.front();
				items.pop();
			}
			pthread_mutex_unlock(&mutex);
		}
		break;
	}
	return ret;
}



template<class W, class JOB>
WorkerPool<W, JOB>::WorkerPool(const char *name){
	this->name = name;
	this->started = false;
	this->should_stop = false;
}

template<class W, class JOB>
WorkerPool<W, JOB>::~WorkerPool(){
	if(started){
		stop();
		for(int i=0; i<tids.size(); i++){
			pthread_join(tids[i], NULL);
		}
	}
}

template<class W, class JOB>
int WorkerPool<W, JOB>::push(JOB job){
	return this->jobs.push(job);
}

template<class W, class JOB>
int WorkerPool<W, JOB>::pop(JOB *job){
	return this->results.pop(job);
}

template<class W, class JOB>
void* WorkerPool<W, JOB>::_run_worker(void *arg){
	struct run_arg *p = (struct run_arg*)arg;
	int id = p->id;
	WorkerPool *tp = p->tp;
	SAFE_DELETE(p);

	W w(tp->name);
	Worker *worker = (Worker *)&w;
	worker->id = id;
	worker->init();
	while(!tp->should_stop){
		JOB job;
		int pop_ret = tp->jobs.pop(&job);
		if(-1 == pop_ret){
			fprintf(stderr, "jobs.pop error\n");
			break;
		}
		if(0 == pop_ret){
			continue;
		}
		worker->proc(job);
		if(tp->results.push(job) == -1){
			fprintf(stderr, "results.push error\n");
			break;
		}
	}
	worker->destroy();
	return (void *)NULL;
}

template<class W, class JOB>
int WorkerPool<W, JOB>::start(int num_workers){
	this->num_workers = num_workers;
	if(started){
		return 0;
	}
	int err;
	pthread_t tid;
	for(int i=0; i<num_workers; i++){
		struct run_arg *arg = new(std::nothrow) run_arg();
		ASSERT_SYS(NULL != arg, "try to new run_arg failed, no memory");
		arg->id = i;
		arg->tp = this;

		err = pthread_create(&tid, NULL, &WorkerPool::_run_worker, arg);
		if(err != 0){
			fprintf(stderr, "can't create thread: %s\n", strerror(err));
		}else{
			tids.push_back(tid);
		}
	}
	started = true;
	return 0;
}

template<class W, class JOB>
int WorkerPool<W, JOB>::stop(){
	this->should_stop = true;
	return 0;
}

#endif


