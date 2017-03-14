/**
* @file server.cpp
* @Brief main server logic
* @author Youzu-BigApp
* @version 1.0.0
* @date 2016-03-10
*/
#include "server.h"
#include "../util/strings.h"
#include "../util/file.h"
#include "../util/config.h"
#include "../util/log.h"
#include "../util/ip_filter.h"
#include "link.h"
#include <vector>

#include "server.h"
#include "../util/strings.h"
#include "../util/file.h"
#include "../util/config.h"
#include "../util/log.h"
#include "../util/ip_filter.h"
#include "../util/exception.h"
#include "../util/tools.h"
#include "../util/signal.h"
#include "link.h"
#include <vector>

#define TICK_INTERVAL          100 // ms
#define STATUS_REPORT_TICKS    (300 * 1000/TICK_INTERVAL) // second
static const int READER_THREADS = 10;
static const int WRITER_THREADS = 1;

volatile bool quit = false;
volatile uint32_t g_ticks = 0;

void signal_handler(int sig){
	switch(sig){
		case SIGTERM:
		case SIGINT:
			log_info("%s", "signal term or signal int received, program is going to die.");
			quit = true;
			break;
		case SIGALRM:
			struct timeval tm;
			gettimeofday(&tm, NULL);
			srand(unsigned(tm.tv_sec - tm.tv_usec));
			g_ticks++;
			break;
	}
}

NetworkServer::NetworkServer(){
	num_readers = READER_THREADS;
	num_writers = WRITER_THREADS;

	tick_interval = TICK_INTERVAL;
	status_report_ticks = STATUS_REPORT_TICKS;

	serv_link = NULL;
	link_count = 0;
	long_connect = false;
	magic_num = 0;
	provider[0] = 0x00;

	fdes = new(std::nothrow) Fdevents();
	ASSERT_SYS(NULL != fdes, "%s", "try to new Fdevents failed, no memory");
	ip_filter = new(std::nothrow) IpFilter();
	ASSERT_SYS(NULL != ip_filter, "%s", "try to new ip_filter failed, no memory");

	writer = NULL;
	reader = NULL;
	
	deal_with_signals(signal_handler, 5, SIGPIPE, SIGINT, SIGTERM, SIGQUIT, SIGALRM);
#ifndef __CYGWIN__
	{
		struct itimerval tv;
		tv.it_interval.tv_sec = (TICK_INTERVAL / 1000);
		tv.it_interval.tv_usec = (TICK_INTERVAL % 1000) * 1000;
		tv.it_value.tv_sec = 1;
		tv.it_value.tv_usec = 0;
		setitimer(ITIMER_REAL, &tv, NULL);
	}
#endif
}

NetworkServer::~NetworkServer(){
	SAFE_DELETE(serv_link);
	SAFE_DELETE(fdes);
	SAFE_DELETE(ip_filter);

	if(writer){
		writer->stop();
		SAFE_DELETE(writer);
	}
	if(reader){
		reader->stop();
		SAFE_DELETE(reader);
	}
}

NetworkServer* NetworkServer::init(const Config &conf, int num_readers, int num_writers){
	static bool inited = false;
	if(inited){
		return NULL;
	}
	inited = true;

	NetworkServer *serv = new (std::nothrow) NetworkServer();
	ASSERT_SYS(NULL != serv, "try to new NetworkServer failed, no memory");
	int32_t _num_readers = conf.get_num("server.num_readers");
	int32_t _num_writers = conf.get_num("server.num_writers");
	if(_num_readers > 0){
		serv->num_readers = _num_readers;
	}
	if(_num_writers > 0){
		serv->num_writers = _num_writers;
	}
	if(num_readers >= 0){
		serv->num_readers = num_readers;
	}
	if(num_writers >= 0){
		serv->num_writers = num_writers;
	}
	log_info("numer_readers: %d, num_writers: %d.", serv->num_readers, serv->num_writers);

	uint32_t mn = conf.get_num("server.magic_num");
	if(0 != mn){
		serv->magic_num = mn;
	}
	const char *pd = conf.get_str("server.provider");
	if(0x00 != pd[0]){
		snprintf(serv->provider, sizeof(serv->provider), "%s", pd);
	}
	log_info("magic_num: %u, provider: %s.", serv->magic_num, serv->provider);
	
	// init ip_filter
	{
		Config *cc = (Config *)conf.get("server");
		if(cc != NULL){
			std::vector<Config *> *children = &cc->children;
			std::vector<Config *>::iterator it;

			for(it = children->begin(); it != children->end(); it++){
				if((*it)->key == "allow"){
					const char *ip = (*it)->str();
					log_info("    allow %s", ip);
					serv->ip_filter->add_allow(ip);
				}
				if((*it)->key == "deny"){
					const char *ip = (*it)->str();
					log_info("    deny %s", ip);
					serv->ip_filter->add_deny(ip);
				}
			}
		}
	}

	{ // server
		const char *ip = conf.get_str("server.ip");
		int port = conf.get_num("server.port");
		if(ip == NULL || ip[0] == '\0'){
			ip = "127.0.0.1";
		}
		int block_queue_size = conf.get_num("server.block_queue_size");
		if(0 == block_queue_size){
			block_queue_size = 1024;
		}
		serv->serv_link = Link::listen(ip, port, block_queue_size);
		if(serv->serv_link == NULL){
			log_fatal("error opening server socket! %s", strerror(errno));
			fprintf(stderr, "error opening server socket! %s\n", strerror(errno));
			exit(1);
		}
		log_info("server listen on %s:%d", ip, port);
		serv->long_connect = !!conf.get_num("server.long_connection");
		log_info("server support long connection [ %s ]", (serv->long_connect ? "true" : "false"));		
	}
	return serv;
}

void NetworkServer::serve(){
	{ 
		auto_mask mask(3, SIGPIPE, SIGINT, SIGTERM, SIGQUIT);
		writer = new (std::nothrow)ProcWorkerPool("writer");
		ASSERT_SYS(NULL != writer, "try to new ProcWorkerPool failed, no memory");
	    writer->start(num_writers);
	    reader = new (std::nothrow)ProcWorkerPool("reader");
		ASSERT_SYS(NULL != reader, "try to new ProcWorkerPool failed, no memory");
	    reader->start(num_readers);
	}
	ready_list_t ready_list;
	ready_list_t ready_list_2;
	ready_list_t::iterator it;
	const Fdevents::events_t *events;

	fdes->set(serv_link->fd(), FDEVENT_IN, 0, serv_link);
	fdes->set(this->reader->fd(), FDEVENT_IN, 0, this->reader);
	fdes->set(this->writer->fd(), FDEVENT_IN, 0, this->writer);

	uint32_t last_ticks = g_ticks;
	while(!quit){
		if((uint32_t)(g_ticks - last_ticks) >= STATUS_REPORT_TICKS){
			last_ticks = g_ticks;
			log_info("server is running, links: %d", this->link_count);
		}
		ready_list.swap(ready_list_2);
		ready_list_2.clear();

		if(!ready_list.empty()){
			events = fdes->wait(0);
		}else{
			events = fdes->wait(50);
		}
		if(events == NULL){
			log_fatal("events.wait error: %s", strerror(errno));
			break;
		}
		
		for(int i=0; i<(int)events->size(); i++){
			const Fdevent *fde = events->at(i);
			if(fde->data.ptr == serv_link){
				Link *link = accept_link();
				if(link){
					this->link_count ++;				
					log_debug("new link from %s:%d, fd: %d, links count: %d",
							link->remote_ip, link->remote_port, 
							link->fd(), this->link_count);

					fdes->set(link->fd(), FDEVENT_IN, 1, link);
				}
			}else if(fde->data.ptr == this->reader || fde->data.ptr == this->writer){
				ProcWorkerPool *worker = (ProcWorkerPool *)fde->data.ptr;
				ProcJob *job = NULL;
				if(worker->pop(&job) == 0){
					log_fatal("reading result from workers error!");		
					exit(0);
				}
				if(proc_result(job, &ready_list) == PROC_ERROR){
					//
				}

			}else{
				proc_client_event(fde, &ready_list);
			}
		}

		for(it = ready_list.begin(); it != ready_list.end(); it ++){
			Link *link = *it;
			if(link->error()){
				this->link_count --;
				fdes->del(link->fd());
				SAFE_DELETE(link);
				continue;
			}

			if(link->readed >= sizeof(head_t) && 0x00 == link->get_basic_string()[0]){
				if(0 == link->request.head.logid){
					link->request.head.logid = pow(2, 64) * (rand() / (RAND_MAX + 1.0));
				}
				link->add_basic_info("client_ip", link->remote_ip);
				link->add_basic_info("client_port", link->remote_port);
				link->add_basic_info("logid", link->request.head.logid);
				link->add_basic_info("method", link->request.head.method);
				link->add_basic_info("magic_num", link->request.head.magic_num);
			}
			if(link->readed >= sizeof(head_t) && link->request.head.bodylen > MAX_PACKAGE_SIZE){
				log_frame("[ %s ]fd: %d, body len is too big, give up reading[ body len: %u, max: %u ].", 
						link->get_basic_string(), link->request.head.bodylen, MAX_PACKAGE_SIZE);
				fdes->del(link->fd());
			}else if(link->readed < sizeof(head_t) || link->request.head.bodylen + sizeof(head_t) > link->readed){
				//head has not been read over or body has not been read over, continue to read
				log_frame("[ %s ]fd: %d, readed: %d, bodylen: %u, continue to read", 
						(link->readed >= sizeof(head_t) ? link->get_basic_string() : ""), link->fd(), 
						link->readed, link->request.head.bodylen);
				fdes->set(link->fd(), FDEVENT_IN, 1, link);
				continue;
			}else{
				log_frame("[ %s ]fd: %d, readed: %d, bodylen: %u, read data finished", link->get_basic_string(), link->fd(), 
						link->readed, link->request.head.bodylen);
				fdes->del(link->fd());
			}
			link->active_time = millitime();
			
			//prepare response head first
			link->response.head = link->request.head;
			snprintf(link->response.head.provider, sizeof(link->response.head.provider), "%s", provider);
			link->response.head.bodylen = 0;

			ProcJob *job = new (std::nothrow)ProcJob();
			ASSERT_SYS(NULL != job, "try to new ProcJob failed, no memory");
			job->link = link;
			job->req = &link->request;
			int result = this->proc(job);

			if(result == PROC_THREAD){
				fdes->del(link->fd());
				continue;
			}
			if(result == PROC_BACKEND){
				job->req = &link->request;
				this->link_count --;
				continue;

			}
			if(proc_result(job, &ready_list_2) == PROC_ERROR){
				//
			}

		} 
		// end foreach ready link
	}
}

Link* NetworkServer::accept_link(){
	Link *link = serv_link->accept();

	if(link == NULL){
		log_error("accept failed! %s", strerror(errno));
		return NULL;
	}
	if(!ip_filter->check_pass(link->remote_ip)){
		log_warn("ip_filter deny link from %s:%d", link->remote_ip, link->remote_port);
		SAFE_DELETE(link);
		return NULL;
	}

	link->nodelay();
	link->noblock();
	link->create_time = millitime();
	link->active_time = link->create_time;
	return link;
}

int NetworkServer::proc_result(ProcJob *job, ready_list_t *ready_list){
	Link *link = job->link;
	int result = job->result;

	log_debug("[%s] process finished, wait time:%.3f,proc time:%.3f, method: %s, status: %u",
			job->link->get_basic_string(), job->time_wait, job->time_proc,
			job->req->head.method,
			job->link->response.head.status);
	if(job->cmd){
		job->cmd->calls += 1;
		job->cmd->time_wait += job->time_wait;
		job->cmd->time_proc += job->time_proc;
	}
	SAFE_DELETE(job);
	int len = 0;
	if(result == PROC_ERROR){
		log_info("[%s] fd: %d, proc error, maybe network is unavailable, delete link", link->get_basic_string(), link->fd());
		goto proc_err;
	}
	//try to write for one time
	//if there is no writing buffer, 0 will be returned
	len = link->write();
	if(len < 0){
		log_warn("[%s] fd: %d, write: %d, delete link", link->get_basic_string(), link->fd(), len);
		goto proc_err;
	}

	if(link->response.head.bodylen + sizeof(head_t) > link->writed){
		//need more writing operation
		fdes->set(link->fd(), FDEVENT_OUT, 1, link);
	}else{
		if(!long_connect || 0 != link->response.head.status){
			//TODO: for short connection
			link_count--;
			fdes->del(link->fd());
			log_frame("[%s] short connection, close [ fd: %d ]", link->get_basic_string(), link->fd());
			SAFE_DELETE(link);
		}else{
			//TODO: long connection£¬ continue to read data
			link->readed = link->writed = 0;
			link->request.reset();
			link->response.reset();
			link->reset_basic_string();
			fdes->set(link->fd(), FDEVENT_IN, 1, link);
			log_frame("[%s] long connection, try to receive request next time [ fd: %d ]", link->get_basic_string(), link->fd());
		}
	}
	return PROC_OK;
proc_err:
	this->link_count --;
	fdes->del(link->fd());
	SAFE_DELETE(link);
	return PROC_ERROR;
}

int NetworkServer::proc_client_event(const Fdevent *fde, ready_list_t *ready_list){
	Link *link = (Link *)fde->data.ptr;
	if(fde->events & FDEVENT_ERR){
		ready_list->push_back(link);
		log_warn("fd: %d, wait error, delete link", link->fd());
		link->mark_error();
		return 0;
	}
	if(fde->events & FDEVENT_IN){
		ready_list->push_back(link);
		if(link->error()){
			return 0;
		}
		int len = link->read();
		if(len <= 0){
			if(link->readed < sizeof(head_t) || link->request.head.bodylen < MAX_PACKAGE_SIZE){
				log_frame("fd: %d, read: %d, delete link [ readed: %d, head_size: %d, bodylen: %u, max: %d ]", 
						link->fd(), len, link->readed, sizeof(head_t), link->request.head.bodylen, MAX_PACKAGE_SIZE);
				link->mark_error();
			}
			return 0;
		}
	}
	if(fde->events & FDEVENT_OUT){
		if(link->error()){
			return 0;
		}
		int len = link->write();
		if(len < 0){
			log_warn("[%s] fd: %d, write: %d, delete link", link->get_basic_string(), link->fd(), len);
			link->mark_error();
			return 0;
		}
		if(link->response.head.bodylen + sizeof(head_t) <= link->writed){
			log_frame("[%s] fd: %d, write: %d, finish writing", link->get_basic_string());
			fdes->clr(link->fd(), FDEVENT_OUT);
		}
		if(!long_connect || 0 != link->response.head.status){
			link_count--;
			fdes->del(link->fd());
			log_frame("[%s] short connection, close [ fd: %d ]", link->get_basic_string(), link->fd());
			SAFE_DELETE(link);
		}else{
			link->readed = link->writed = 0;
			link->request.reset();
			link->response.reset();
			link->reset_basic_string();
			fdes->set(link->fd(), FDEVENT_IN, 1, link);
			log_frame("[%s] long connection, try to receive request next time [ fd: %d ]", link->get_basic_string(), link->fd());
		}
	}
	return 0;
}

int NetworkServer::proc(ProcJob *job){
	job->serv = this;
	job->result = PROC_OK;
	job->stime = millitime();

	const Protoc *req = job->req;

	do{
		Command *cmd = proc_map.get_proc(req->head.method);
		if(!cmd){
			log_warn("[%s] invalid method [ method: %s ].", job->link->get_basic_string(), req->head.method);
			job->link->response.head.status = 1;  //method does not exist
			break;
		}
		if(job->link->request.head.magic_num != magic_num){
			log_warn("[%s] invalid magic num [ magic_num: %u, config: %u ].", 
					job->link->get_basic_string(), req->head.magic_num, magic_num);
			job->link->response.head.status = 2; //invalid magic num
			break;
		}
		if(job->link->request.head.bodylen > MAX_PACKAGE_SIZE){
			log_warn("[%s] request body too big [ body len: %u, max: %u ].", 
					job->link->get_basic_string(), job->link->request.head.bodylen, MAX_PACKAGE_SIZE);
			job->link->response.head.status = 3; //invalid magic num
			break;
		}
		job->cmd = cmd;
		if(cmd->flags & Command::FLAG_THREAD){
			if(cmd->flags & Command::FLAG_WRITE){
				log_frame("[%s] command with write property, push it to backend thread [ logid: %llu, method: %s ]",
						job->link->get_basic_string(), job->link->response.head.logid, job->link->response.head.method);
				if(-1 == writer->push(job)){
					log_warn("[%s] push job into writer queue failed.", job->link->get_basic_string());
					job->link->response.head.status = 5;  //other system error
				}
			}else{
				log_frame("[%s] command with read property, push it to backend thread [ logid: %llu, method: %s ]",
						job->link->get_basic_string(), job->link->response.head.logid, job->link->response.head.method);
				if(-1 == reader->push(job)){
					log_warn("[%s] push job into reader queue failed.", job->link->get_basic_string());
					job->link->response.head.status = 5;   //other system error
				}
			}
			return PROC_THREAD;
		}
		log_frame("[%s] command will be processed in main thread [ logid: %llu, method: %s ]",
				job->link->get_basic_string(), job->link->response.head.logid, job->link->response.head.method);
		proc_t p = cmd->proc;
		job->time_wait = 1000 * (millitime() - job->stime);
		job->result = (*p)(this, job->link, *req, &job->link->response);
		job->time_proc = 1000 * (millitime() - job->stime) - job->time_wait;
	}while(0);

	return job->result;
}

