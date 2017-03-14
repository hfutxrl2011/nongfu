#ifndef SSDB_API_IMPL_CPP
#define SSDB_API_IMPL_CPP

#include "SSDB_client.h"
#include "net/link.h"

namespace ssdb{

class ClientImpl : public Client{
private:
	friend class Client;
	
	Link *link;
	std::vector<std::string> resp_;
public:
	ClientImpl();
	~ClientImpl();

	virtual void* request(const std::string &cmd, const void *req, uint32_t len, void *res, uint32_t &res_len);
	int get(const void *req, uint32_t req_len, void *res, uint32_t &res_len);
	int set(const void *req, uint32_t req_len, void *res, uint32_t &res_len);
};

}; // namespace ssdb

#endif
