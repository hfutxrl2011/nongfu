 
#include "worker.h"
#include "link.h"
#include "proc.h"
#include "../util/log.h"
#include "../include.h"

ProcWorker::ProcWorker(const std::string &name){
	this->name = name;
}

void ProcWorker::init(){
	log_info("%s %d init [ thread_id: %lu ]", this->name.c_str(), this->id, pthread_self());
}

int ProcWorker::proc(ProcJob *job){
	const Protoc *req = job->req;
	
	proc_t p = job->cmd->proc;
	job->time_wait = 1000 * (millitime() - job->stime);
	job->result = (*p)(job->serv, job->link, *req, &job->link->response);
	job->time_proc = 1000 * (millitime() - job->stime) - job->time_wait;

	if(job->cmd->flags & Command::FLAG_READ){
		//try to write for one time
		//if there is no writing buffer, 0 will be returned
		int len = job->link->write();
		if(len < 0){
			job->result = PROC_ERROR;
		}
	}
	return 0;
}

