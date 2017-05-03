 
#ifndef UTIL_THREAD_MEM_H_
#define UTIL_THREAD_MEM_H_

#include <map>
#include <string>
#include <stdint.h>
#include <pthread.h>

typedef struct _thread_mem_item_t
{
	pthread_key_t thread_key;
	uint32_t len;
}thread_mem_item_t;
class ThreadMemory
{
public:
	static std::map<std::string, thread_mem_item_t> item_map;
	static void register_thread_memory(std::string name, uint32_t len);
	static void create_keys();
	static void destory_keys();
	static void* get_thread_memory(std::string name);
private:
	static void _create_keys();
};
#endif
