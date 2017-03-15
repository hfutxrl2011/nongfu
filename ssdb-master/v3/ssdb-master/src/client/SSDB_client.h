 
#ifndef SSDB_API_CPP
#define SSDB_API_CPP

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <inttypes.h>
#include <string>
#include <vector>
#include <map>

namespace ssdb{
class Client{
public:
	static Client* connect(const char *ip, int port);
	static Client* connect(const std::string &ip, int port);
	Client(){};
	virtual ~Client(){};

	virtual void *request(const std::string &cmd, const void *req, uint32_t len, void *res, uint32_t &res_len) = 0;
	virtual int get(const void *req, uint32_t req_len, void *res, uint32_t &res_len) = 0;
	virtual int set(const void *req, uint32_t req_len, void *res, uint32_t &res_len) = 0;
private:
	Client(const Client&);
	void operator=(const Client&);
};

}; // namespace ssdb

#endif
