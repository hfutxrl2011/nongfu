 
#ifndef AI_API_CPP
#define AI_API_CPP

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <inttypes.h>
#include <string>
#include <vector>
#include <map>
#include "game.pb.h"
#include "actions.pb.h"
using namespace pb;
using namespace google::protobuf;

namespace ai{
class Client{
public:
	static Client* connect(const char *ip, int port);
	static Client* connect(const std::string &ip, int port);
	Client(){};
	std::string currentState;
	virtual ~Client(){};
	virtual int getLinkFd() = 0;

	virtual void *request(uint32_t cmd, const void *req, uint32_t len, void *res, uint32_t &res_len) = 0;
	virtual int roomlogin(const void *req, uint32_t req_len, void *res, uint32_t &res_len) = 0;
	virtual int reqmove(const void *req, uint32_t req_len) = 0;
	virtual int reqstop(const void *req, uint32_t req_len) = 0;
	virtual int reqrelive(const void *req, uint32_t req_len) = 0;
	virtual int reqreleasespell(const void *req, uint32_t req_len) = 0;
	virtual int reqspellup(const void *req, uint32_t req_len) = 0;
	virtual int readNotify(uint32_t &cmd, void *res, uint32_t &res_len) = 0;
	virtual int readFrameNotify(void *res, uint32_t &res_len) = 0;
	virtual int readFrameData(uint32_t cmd, const ::std::string &res) = 0;
	virtual int readPlayerAddNotify(void *res, uint32_t &res_len) = 0;
	virtual int readPlayerRemoveNotify(void *res, uint32_t &res_len) = 0;
	virtual int readFrameDataMove(const ::std::string &res) = 0;//移动数据
	virtual int readSceneItemCreate(const ::std::string &res) = 0;
	virtual int readSceneItemRemove(const ::std::string &res) = 0;
	virtual int readBuffStart(const ::std::string &res) = 0;
	virtual int readBuffRemove(const ::std::string &res) = 0;
	virtual int readSceneBaseState(const ::std::string &res) = 0;
	virtual int readLevelUp(const ::std::string &res) = 0;
	virtual int readDeath(const ::std::string &res) = 0;
	virtual int readFrameSpellLevel(const ::std::string &res) = 0;
	
	virtual int readLADDERNotify(void *res, uint32_t &res_len) = 0;
	virtual int readDRAGNotify(void *res, uint32_t &res_len) = 0;

	virtual int setGameState(RoomLoginRes *currentState) = 0;
	virtual RoomLoginRes* getGameState() = 0;
	
private:
	Client(const Client&);
	void operator=(const Client&);
};

}; // namespace ai

#endif
