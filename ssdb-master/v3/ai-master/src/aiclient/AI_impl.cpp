#include "AI_impl.h"
#include "util/strings.h"
#include <signal.h>

#include "game.pb.h"

using namespace pb;

namespace ai{

ClientImpl::ClientImpl(){
	link = NULL;
}

ClientImpl::~ClientImpl(){
	if(link){
		delete link;
	}
}

Client* Client::connect(const char *ip, int port){
	return Client::connect(std::string(ip), port);
}

Client* Client::connect(const std::string &ip, int port){
	static bool inited = false;
	if(!inited){
		inited = true;
		signal(SIGPIPE, SIG_IGN);
	}
	ClientImpl *client = new ClientImpl();
	client->link = Link::connect(ip.c_str(), port);
	if(client->link == NULL){
		delete client;
		return NULL;
	}
	return client;
}

int ClientImpl::getLinkFd(){
	return link->fd();
}

void* ClientImpl::request(uint32_t cmd, const void *req, uint32_t len, void *res, uint32_t &res_len){
	fprintf(stderr, "\n request write start [ cmd: %d, req_len: %u ].\n", cmd, len);
	doWriteRequest(cmd,req,len);
	
	fprintf(stderr, "request read start [ cmd: %d, res_len: %u ].\n", cmd, res_len);
	uint32_t readcmd = 0;
	doReadRequest(readcmd,res, res_len);
	fprintf(stderr, "request read end [ readcmd: %d, req_len: %u ].\n", readcmd, len);
	return res;
}

void* ClientImpl::doReadRequest(uint32_t &cmd,void *res, uint32_t &res_len){
	uint32_t res_buf_len = res_len;
	res_len = 0;
	link->readed = link->writed = 0;
	int fd = link->fd();
	//r head
	head_game res_head;
	int left = sizeof(res_head);
	int total = left;
	while(left > 0){
		int rret = ::read(fd, reinterpret_cast<char *>(&res_head) + total - left, left);
		if(-1 == rret){
			if(EINTR == errno){
				continue;
			}
			fprintf(stderr, "read failed [ fd: %d, error: %s ].\n", fd, strerror(errno));
			return NULL;
		}
		left -= rret;
	}
	if(res_head.Flags != 0){
		fprintf(stderr, "request returned with error status [ status: %u ].\n", res_head.Flags);
		return NULL;
	}
	fprintf(stderr, "request run [ length: %u,command: %u ].\n",  res_head.Length,res_head.Command);
	
	cmd = res_head.Command;
	left = res_head.Length;
	total = left;
	if(res_buf_len < total){
		fprintf(stderr, "response data is too big [ res_buf_len: %u, total: %u ].\n", res_buf_len, total);
		return NULL;
	}
	
	//r body
	while(left > 0){
		int rret = ::read(fd, reinterpret_cast<char *>(res) + total - left, left);
		if(-1 == rret){
			if(EINTR == errno){
				continue;
			}
			fprintf(stderr, "read failed [ fd: %d, error: %s ].\n", fd, strerror(errno));
			return NULL;
		}
		left -= rret;
	}
	res_len = res_head.Length;
	fprintf(stderr, "request finished 123 [ method: %s, res_len: %u ].\n", "doread", res_len);	
	return res;
}

void ClientImpl::doWriteRequest(uint32_t cmd, const void *req, uint32_t len){
	head_game req_head;
	req_head.Flags = 0;
	req_head.Length = len;
	req_head.Command = cmd;
	
	fprintf(stderr, "request start [ cmd: %d, req_len: %u ].\n", cmd, len);
	
	link->readed = link->writed = 0;
	int fd = link->fd();
	int left = sizeof(req_head);
	int total = left;
	
	//w head
	while(left > 0){
		int wret = ::write(fd, reinterpret_cast<const char *>(&req_head) + total - left, left);
		if(-1 == wret){
			if(EINTR == errno){
				fprintf(stderr, "write failed [ fd: %d, error: %s ].\n", fd, strerror(errno));
				continue;
			}
			//return NULL;
		}
		left -= wret;
	}
	
	//w body
	left = len;
	total = left;
	while(left > 0){
		int wret = ::write(fd, reinterpret_cast<const char *>(req) + total - left, left);
		if(-1 == wret){
			if(EINTR == errno){
				continue;
			}
			fprintf(stderr, "write failed [ fd: %d, error: %s ].\n", fd, strerror(errno));
			//return NULL;
		}
		left -= wret;
	}
}

int ClientImpl::roomlogin(const void *req, uint32_t req_len, void *res, uint32_t &res_len)
{
	uint32_t cmd = ROOM_LOGIN;//room login
	if(NULL == request(cmd, req, req_len, res, res_len)){
		return -1;
	}	
	return 0;
}

int ClientImpl::reqmove(const void *req, uint32_t req_len)
{
	uint32_t cmd = ROOM_PLAYER_MOVE;
	doWriteRequest(cmd, req, req_len);
	return -1;
}
int ClientImpl::reqstop(const void *req, uint32_t req_len)
{
	uint32_t cmd = ROOM_PLAYER_STOP;
	doWriteRequest(cmd, req, req_len);	
	return 0;
}
int ClientImpl::reqrelive(const void *req, uint32_t req_len)
{
	uint32_t cmd = ROOM_PLAYER_RELIVE;
	doWriteRequest(cmd, req, req_len);	
	return 0;
}
int ClientImpl::reqreleasespell(const void *req, uint32_t req_len)
{
	uint32_t cmd = ROOM_PLAYER_SPELL;
	doWriteRequest(cmd, req, req_len);
	return 0;
}
int ClientImpl::reqspellup(const void *req, uint32_t req_len)
{
	uint32_t cmd = ROOM_SPELL_LEVEL_UP;//room login
	doWriteRequest(cmd, req, req_len);
	return 0;
}




int ClientImpl::readNotify(uint32_t &cmd, void *res, uint32_t &res_len)
{
	doReadRequest(cmd,res, res_len);
	fprintf(stderr, "read notice do read [ cmd: %d ].\n", cmd);
	
	switch(cmd)
	{
		case ROOM_FRAME_NOTIFY:
		{	
			readFrameNotify(res,res_len);
			break;
		}
		
		case ROOM_LADDER_NOTIFY:
		{
			readLADDERNotify(res,res_len);
			break;
		}
		/*
		case ROOM_KILLS_NOTIFY:
		{
			
			//break;
		}
		case ROOM_MOVE_FORCE_RES:
		{
			
			//break;
		}
		case ROOM_ADD_PLAYER:
		{
			
			//break;
		}
		case ROOM_REMOVE_PLAYER:
		{
			
			//break;
		}
		*/
		default:
			fprintf(stderr, "read notice failed [ cmd: %d ].\n", cmd);
			break;
	}
	
	return 0;
}

int ClientImpl::readLADDERNotify(void *res, uint32_t &res_len)
{
	RoomLadderNotify fres;
	if(0 == res_len || !fres.ParseFromArray(res, res_len)){
		fprintf(stderr, "[method: %s] parse game result failed.\n", "readLADDERNotify");
	}else{
		std::string str,state,new_state;
		//pbjson::pb2json(&fres, str);
		//pbjson::pb2json(gameState, state);
		fprintf(stderr, "[return str: %s] item size: %d\n", str.c_str(), fres.items_size());
		for (int i = 0; i < fres.items_size(); i++) {
			const RoomLadderNotify::Item& item = fres.items(i);
			
			// 获取message的descriptor
			//const Descriptor* descriptor = gameState->GetDescriptor();
			// 获取message的反射接口，可用于获取和修改字段的值
			//const Reflection* reflection = gameState->GetReflection();
			// 根据字段名查找message的字段descriptor
			//const FieldDescriptor* idField = descriptor->FindFieldByName("id");
			
			//if (NULL != idField) {
				//reflection->SetInt32(gameState, idField, 100);
			//}
			
			//for( int j = 0 ; j < gameState->snap_shot().players_size() ; j++){
				//RoomPlayer* player = gameState->snap_shot()->players(j);
				// 将id设置为100
				//if (NULL != idField) {
				//	reflection->SetInt32(gameState, idField, 100);
				//}
				//if(player->id() == item.id()){
					//player->set_score(item.score());
				//}
			//}
			fprintf(stderr, "[method: %s] parse game result succ [ id: %d,score:%d,kill_times:%d,dead_times:%d ].\n", "p3", item.id(),item.score(),item.kill_times(),item.dead_times());
		}
		//pbjson::pb2json(gameState, new_state);
		//fprintf(stderr, "[method: readLADDERNotify] parse game state:%s,new state:%s",state.c_str(),new_state.c_str());
	}
	return 0;
}

int ClientImpl::readFrameNotify(void *res, uint32_t &res_len)
{
	RoomFrameNotify fres;
	if(0 == res_len || !fres.ParseFromArray(res, res_len)){
		fprintf(stderr, "[method: %s] parse game result failed.\n", "readFrameNotify");
	}else{
		uint32_t frameid = fres.frame();
		fprintf(stderr, "[method: %s] item size: %d,frame:%d.\n", "p1", fres.list_size(),frameid);
		for (int i = 0; i < fres.list_size(); i++) {
			const Frame& frame = fres.list(i);
			fprintf(stderr, "[method: %s] i:%d.\n","p2",i);
			int32_t cmd = frame.cmd();
			const ::std::string data = frame.data();
			readFrameData(cmd,data);
			fprintf(stderr, "[method: %s] parse game result succ [ cmd: %d ].\n", "p3", cmd);
		}
		
	}
	return 0;
}

int ClientImpl::readFrameData(uint32_t cmd, const ::std::string &res)
{
	fprintf(stderr, "readFrameData [ cmd: %d ].\n", cmd);
	
	switch(cmd)
	{
		case CMD_MOVE:
		{	
			readFrameDataMove(res);
			break;
		}
		default:
			fprintf(stderr, "read readFrameData failed [ cmd: %d ].\n", cmd);
			break;
	}
	
	return 0;
}

int ClientImpl::readFrameDataMove(const ::std::string &res)
{
	fprintf(stderr, "read readFrameDataMove.\n");
	FrameMove framemove;
	if(!framemove.ParseFromString(res)){
		fprintf(stderr, "[method: %s] parse game result failed.\n", "readFrameDataMove");
	}else{
		Vector res_pos = framemove.pos();
		fprintf(stderr, "[method: %s] parse game x:%d,z:%d.\n", "readFrameDataMove",res_pos.x(),res_pos.z());
		Vector res_dir = framemove.dir();
		fprintf(stderr, "[method: %s] parse game x:%d,z:%d.\n", "readFrameDataMove",res_dir.x(),res_dir.z());
		fprintf(stderr, "[method: %s] parse game id:%d.\n", "readFrameDataMove",framemove.id());
	}
	return 0;
}

int ClientImpl::setGameState(RoomLoginRes *currentState)
{
		gameState = currentState;
		return 1;
}

RoomLoginRes* ClientImpl::getGameState(){
	return gameState;
}


}; // namespace ai
