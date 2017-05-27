#ifndef AI_API_IMPL_CPP
#define AI_API_IMPL_CPP

#include "AI_client.h"
#include "net/link.h"
#include "util/log.h"
#include "pbjson.hpp"
#include "game.pb.h"
#include "actions.pb.h"

namespace ai{

class ClientImpl : public Client{
private:
	friend class Client;
	
	Link *link;
	std::vector<std::string> resp_;
	RoomLoginRes *gameState1;
public:
	
	//std::string currentState;
	ClientImpl();
	~ClientImpl();
	
	int getLinkFd();
	virtual void* request(uint32_t cmd, const void *req, uint32_t len, void *res, uint32_t &res_len);
	virtual void* doReadRequest(uint32_t &cmd, void *res, uint32_t &res_len);
	virtual void doWriteRequest(uint32_t cmd, const void *req, uint32_t len);
	int roomlogin(const void *req, uint32_t req_len, void *res, uint32_t &res_len);
	int reqmove(const void *req, uint32_t req_len);
	int reqstop(const void *req, uint32_t req_len);
	int reqrelive(const void *req, uint32_t req_len);
	int reqreleasespell(const void *req, uint32_t req_len);
	int reqspellup(const void *req, uint32_t req_len);
	
	int readNotify(uint32_t &cmd, void *res, uint32_t &res_len);
	int readFrameNotify(void *res, uint32_t &res_len);
	int readFrameData(uint32_t cmd, const ::std::string &res);
	int readPlayerAddNotify(void *res, uint32_t &res_len);
	int readPlayerRemoveNotify(void *res, uint32_t &res_len);
	
	int readFrameDataMove(const ::std::string &res);
	int readSceneItemCreate(const ::std::string &res);
	int readSceneItemRemove(const ::std::string &res);
	int readBuffStart(const ::std::string &res);
	int readBuffRemove(const ::std::string &res);
	int readSceneBaseState(const ::std::string &res);
	int readLevelUp(const ::std::string &res);
	int readDeath(const ::std::string &res);
	int readFrameSpellLevel(const ::std::string &res);
	
	int readLADDERNotify(void *res, uint32_t &res_len);
	int readDRAGNotify(void *res, uint32_t &res_len);
	
	int setGameState(RoomLoginRes *currentState);
	RoomLoginRes* getGameState();
	void RemoveFromRepeatedField(
    const google::protobuf::Reflection *reflection,
    const google::protobuf::FieldDescriptor *field,
    google::protobuf::Message *message,
    int row,
    int count);
	
	
};

}; // namespace ai

#endif
