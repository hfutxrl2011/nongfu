#ifndef AI_API_IMPL_CPP
#define AI_API_IMPL_CPP

#include "AI_client.h"
#include "net/link.h"

namespace ai{

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
	int getgame(const void *req, uint32_t req_len, void *res, uint32_t &res_len);
	int set(const void *req, uint32_t req_len, void *res, uint32_t &res_len);
};

}; // namespace ai

#endif
