 
#include <errno.h>
#include "log.h"
#include "exception.h"
#include "thread_mem.h"
#include "tools.h"

pthread_once_t random_is_initialized = PTHREAD_ONCE_INIT;
std::map<std::string, thread_mem_item_t> ThreadMemory::item_map;

void ThreadMemory::destory_keys()
{
	for(std::map<std::string, thread_mem_item_t>::iterator it = ThreadMemory::item_map.begin(); it != ThreadMemory::item_map.end(); it++){
		pthread_key_delete(it->second.thread_key);
	}
	ThreadMemory::item_map.clear();
}

void ThreadMemory::register_thread_memory(std::string name, uint32_t len)
{
	errno = 0;
	if(0 >= len){
		THROW_PARAM("invalid param len while register thread memory [ len: %u ].", len);
	}
	std::map<std::string, thread_mem_item_t>::iterator it = ThreadMemory::item_map.find(name);
	if(it != ThreadMemory::item_map.end()){
		ThreadMemory::item_map[name].len = len;
	}else{
		thread_mem_item_t tmp;
		tmp.len = len;
		ThreadMemory::item_map[name] = tmp;
	}
	log_info("register thread memory succ [ name: %s, len: %u ].", name.c_str(), len);
}

void ThreadMemory::create_keys()
{
	if(0 == ThreadMemory::item_map.size()){
		return ;
	}
	if(-1 == pthread_once(&random_is_initialized, &_create_keys)){
		THROW_SYS("try to create keys failed [ error: %m ]");	
	}
}

void ThreadMemory::_create_keys()
{
	for(std::map<std::string, thread_mem_item_t>::iterator it = ThreadMemory::item_map.begin(); it != ThreadMemory::item_map.end(); it++){
		if(-1 == pthread_key_create(&it->second.thread_key, free)){
			THROW_SYS("try to create pthread_key failed %m");
		}
	}
	log_info("create keys succ [ keys num: %lu ].", ThreadMemory::item_map.size());
}

void *ThreadMemory::get_thread_memory(std::string name)
{
	std::map<std::string, thread_mem_item_t>::iterator it = ThreadMemory::item_map.find(name);
	if(it == ThreadMemory::item_map.end()){
		THROW_SYS("try to get a thread memory, which has not been registered [ name: %s ].", name.c_str());
	}
	void *tmp = pthread_getspecific(it->second.thread_key);
	if(NULL == tmp){
		tmp = malloc(it->second.len);
		if(NULL == tmp){
			THROW_SYS("try to malloc %u bytes memory for thread failed [ thread: %lu, error: %m ].", it->second.len, pthread_self());
		}
		log_info("malloc %u bytes momory for thread finished [ thread: %lu ].", it->second.len, pthread_self());
		if(-1 == pthread_setspecific(it->second.thread_key, tmp)){
			SAFE_FREE(tmp);
			THROW_SYS("setspecific call failed [ thread: %lu, error: %m ].", pthread_self());
		}
	}

	return tmp;
}

