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

	virtual void* request(uint32_t cmd, const void *req, uint32_t len, void *res, uint32_t &res_len);
	virtual void* doReadRequest(uint32_t &cmd, void *res, uint32_t &res_len);
	virtual void doWriteRequest(uint32_t cmd, const void *req, uint32_t len);
	int roomlogin(const void *req, uint32_t req_len, void *res, uint32_t &res_len);
	int reqmove(const void *req, uint32_t req_len, void *res, uint32_t &res_len);
	int reqstop(const void *req, uint32_t req_len, void *res, uint32_t &res_len);
	int reqrelive(const void *req, uint32_t req_len, void *res, uint32_t &res_len);
	int reqreleasespell(const void *req, uint32_t req_len, void *res, uint32_t &res_len);
	int reqspellup(const void *req, uint32_t req_len, void *res, uint32_t &res_len);
	
	int readNotify(uint32_t &cmd, void *res, uint32_t &res_len);
	int readFrameNotify(void *res, uint32_t &res_len);
	int readFrameData(uint32_t cmd, const ::std::string &res);
	int readFrameDataMove(const ::std::string &res);
	
	int readLADDERNotify(void *res, uint32_t &res_len);
	
};

}; // namespace ai

#endif
