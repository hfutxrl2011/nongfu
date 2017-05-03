 
#ifndef AI_API_CPP
#define AI_API_CPP

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <inttypes.h>
#include <string>
#include <vector>
#include <map>

namespace ai{
class Client{
public:
	static Client* connect(const char *ip, int port);
	static Client* connect(const std::string &ip, int port);
	Client(){};
	virtual ~Client(){};

	virtual void *request(uint32_t cmd, const void *req, uint32_t len, void *res, uint32_t &res_len) = 0;
	virtual int roomlogin(const void *req, uint32_t req_len, void *res, uint32_t &res_len) = 0;
	virtual int reqmove(const void *req, uint32_t req_len, void *res, uint32_t &res_len) = 0;
	virtual int readNotify(uint32_t &cmd, void *res, uint32_t &res_len) = 0;
	virtual int readFrameNotify(void *res, uint32_t &res_len) = 0;
	virtual int readFrameData(uint32_t cmd, const ::std::string &res) = 0;
	virtual int readFrameDataMove(const ::std::string &res) = 0;//移动数据
	
	virtual int readLADDERNotify(void *res, uint32_t &res_len) = 0;
	
private:
	Client(const Client&);
	void operator=(const Client&);
};

}; // namespace ai

#endif
