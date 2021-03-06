 
#ifndef NET_PROC_H_
#define NET_PROC_H_

#include <vector>
#include <string>
#include "protoc.h"

class Link;
class NetworkServer;

#define PROC_OK			0
#define PROC_ERROR		-1
#define PROC_THREAD     1
#define PROC_BACKEND	100

#define DEF_PROC(f) int proc_##f(NetworkServer *net, Link *link, const Protoc &req, Protoc *resp)
typedef int (*proc_t)(NetworkServer *net, Link *link, const Protoc &req, Protoc *resp);

struct Command{
	static const int FLAG_READ		= (1 << 0);
	static const int FLAG_WRITE		= (1 << 1);
	static const int FLAG_BACKEND	= (1 << 2);
	static const int FLAG_THREAD	= (1 << 3);

	std::string name;
	int flags;
	proc_t proc;
	uint64_t calls;
	double time_wait;
	double time_proc;
	
	Command(){
		flags = 0;
		proc = NULL;
		calls = 0;
		time_wait = 0;
		time_proc = 0;
	}
};

struct ProcJob{
	int result;
	NetworkServer *serv;
	Link *link;
	Command *cmd;
	double stime;
	double time_wait;
	double time_proc;
	
	const Protoc*req;
	
	ProcJob(){
		result = 0;
		serv = NULL;
		link = NULL;
		cmd = NULL;
		stime = 0;
		time_wait = 0;
		time_proc = 0;
	}
	~ProcJob(){
	}
};

#define GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#if GCC_VERSION >= 403
    #include <tr1/unordered_map>
    typedef std::tr1::unordered_map<std::string, Command *> proc_map_t;
#else
    #ifdef NEW_MAC
        #include <unordered_map>
        typedef std::unordered_map<std::string, Command *> proc_map_t;
    #else
        #include <ext/hash_map>
        typedef __gnu_cxx::hash_map<std::string, Command *> proc_map_t;
    #endif
#endif

class ProcMap
{
private:
	proc_map_t proc_map;

public:
	ProcMap();
	~ProcMap();
	void set_proc(const std::string &cmd, const char *sflags, proc_t proc);
	void set_proc(const std::string &cmd, proc_t proc);
	Command* get_proc(std::string str);
	
	proc_map_t::iterator begin(){
		return proc_map.begin();
	}
	proc_map_t::iterator end(){
		return proc_map.end();
	}
};



#include "../util/strings.h"

template<class T>
static std::string serialize_req(T &req){
	std::string ret;
	char buf[50];
	for(int i=0; i<req.size(); i++){
		if(i >= 5 && i < req.size() - 1){
			sprintf(buf, "[%d more...]", (int)req.size() - i);
			ret.append(buf);
			break;
		}
		if(((req[0] == "get" || req[0] == "set") && i == 1) || req[i].size() < 50){
			if(req[i].size() == 0){
				ret.append("\"\"");
			}else{
				std::string h = hexmem(req[i].data(), req[i].size());
				ret.append(h);
			}
		}else{
			sprintf(buf, "[%d]", (int)req[i].size());
			ret.append(buf);
		}
		if(i < req.size() - 1){
			ret.append(" ");
		}
	}
	return ret;
}

#endif
