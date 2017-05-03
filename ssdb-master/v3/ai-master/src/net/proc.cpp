 
#include "proc.h"
#include "server.h"
#include "../util/log.h"
#include "../util/tools.h"

ProcMap::ProcMap(){
}

ProcMap::~ProcMap(){
	proc_map_t::iterator it;
	for(it=proc_map.begin(); it!=proc_map.end(); it++){
		SAFE_DELETE(it->second);
	}
	proc_map.clear();
}

void ProcMap::set_proc(const std::string &c, proc_t proc){
	this->set_proc(c, "t", proc);
}

void ProcMap::set_proc(const std::string &c, const char *sflags, proc_t proc){
	Command *cmd = this->get_proc(c);
	if(!cmd){
		cmd = new (std::nothrow) Command();
		ASSERT_SYS(NULL != cmd, "%s", "try to new Command failed, no memory");
		cmd->name = c;
		proc_map[cmd->name] = cmd;
	}
	cmd->proc = proc;
	cmd->flags = 0;
	for(const char *p=sflags; *p!='\0'; p++){
		switch(*p){
			case 'r':
				cmd->flags |= Command::FLAG_READ;
				break;
			case 'w':
				cmd->flags |= Command::FLAG_WRITE;
				cmd->flags |= Command::FLAG_THREAD;
				break;
			case 'b':
				cmd->flags |= Command::FLAG_BACKEND;
				break;
			case 't':
				cmd->flags |= Command::FLAG_THREAD;
				break;
		}
	}
}

Command* ProcMap::get_proc(std::string str){
	proc_map_t::iterator it = proc_map.find(str);
	if(it != proc_map.end()){
		return it->second;
	}
	return NULL;
}
