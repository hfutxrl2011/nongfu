/**
* @file serv.h
* @Brief processors manager
* @author Youzu-BigApp
* @version 1.0.0
* @date 2016-03-10
*/
#ifndef SSDB_SERVER_H_
#define SSDB_SERVER_H_

#include "include.h"
#include <map>
#include <vector>
#include <string>
#include "net/server.h"

class SSDBServer
{
private:
	void reg_procs(NetworkServer *net);

public:
	SSDBServer(const Config &conf, NetworkServer *net);
	~SSDBServer();
};

#endif
