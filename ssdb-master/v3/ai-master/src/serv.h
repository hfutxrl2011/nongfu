 
#ifndef AI_SERVER_H_
#define AI_SERVER_H_

#include "include.h"
#include <map>
#include <vector>
#include <string>
#include "net/server.h"

class AIServer
{
private:
	void reg_procs(NetworkServer *net);

public:
	AIServer(const Config &conf, NetworkServer *net);
	~AIServer();
};

#endif
