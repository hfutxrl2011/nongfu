 
#include "../serv.h"
#include "../net/proc.h"
#include "../net/server.h"

int proc_get(NetworkServer *net, Link *link, const Request &req, Response *resp){
	resp->push_back("ok");
	resp->push_back("1");
	return 0;
}

int proc_set(NetworkServer *net, Link *link, const Request &req, Response *resp){
	resp->push_back("ko");
	resp->push_back("1");
	return 0;
}
