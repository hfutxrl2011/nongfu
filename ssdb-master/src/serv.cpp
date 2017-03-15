 
#include "version.h"
#include "util/log.h"
#include "util/strings.h"
#include "serv.h"
#include "net/proc.h"
#include "net/server.h"

DEF_PROC(get);
DEF_PROC(set);

#define REG_PROC(c, f)     net->proc_map.set_proc(#c, f, proc_##c)

void SSDBServer::reg_procs(NetworkServer *net){
	REG_PROC(get, "rt");
	REG_PROC(set, "wt");
}


SSDBServer::SSDBServer(const Config &conf, NetworkServer *net){
	net->data = this;
	this->reg_procs(net);
}

SSDBServer::~SSDBServer(){
	log_debug("SSDBServer finalized");
}
