 
#ifndef NET_SERVER_H_
#define NET_SERVER_H_

#include "../include.h"
#include <string>
#include <vector>

#include "fde.h"
#include "proc.h"
#include "worker.h"

class Link;
class Config;
class IpFilter;
class Fdevents;

typedef std::vector<Link *> ready_list_t;

class NetworkServer
{
private:
	int tick_interval;
	int status_report_ticks;
	
	Link *serv_link;
	IpFilter *ip_filter;
	Fdevents *fdes;
	bool long_connect;
	char provider[32];
	uint32_t magic_num;

	Link* accept_link();
	int proc_result(ProcJob *job, ready_list_t *ready_list);
	int proc_client_event(const Fdevent *fde, ready_list_t *ready_list);

	int proc(ProcJob *job);

	int num_readers;
	int num_writers;
	ProcWorkerPool *writer;
	ProcWorkerPool *reader;

	NetworkServer();

protected:
	void usage(int argc, char **argv);

public:
	void *data;
	ProcMap proc_map;
	int link_count;

	~NetworkServer();
	
	static NetworkServer* init(const char *conf_file, int num_readers=-1, int num_writers=-1);
	static NetworkServer* init(const Config &conf, int num_readers=-1, int num_writers=-1);
	void serve();
};


#endif
