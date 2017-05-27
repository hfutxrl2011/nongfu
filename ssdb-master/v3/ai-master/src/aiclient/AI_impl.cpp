#include "AI_impl.h"
#include "util/strings.h"
#include <signal.h>

#include "game.pb.h"

using namespace pb;
using namespace google::protobuf;


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
	log_info("readNotify read notice do read [ cmd: %d ].\n", cmd);
	switch(cmd)
	{
		case ROOM_FRAME_NOTIFY:
		{	
			fprintf(stderr, "[method: ROOM_FRAME_NOTIFY] =============================>>>>>>>>>>>\n");
			readFrameNotify(res,res_len);
			break;
		}
		case ROOM_LADDER_NOTIFY:
		{
			fprintf(stderr, "[method: ROOM_LADDER_NOTIFY] =============================>>>>>>>>>>>\n");
			readLADDERNotify(res,res_len);
			break;
		}
		case ROOM_PLAYER_DRAG:
		{
			fprintf(stderr, "[method: ROOM_PLAYER_DRAG] =============================>>>>>>>>>>>\n");
			readDRAGNotify(res,res_len);
			break;
		}
		/*
		case ROOM_KILLS_NOTIFY:
		{
			
			//break;
		}
		*/
		case ROOM_PLAYER_ADD:
		{
			fprintf(stderr, "[method: ROOM_PLAYER_ADD] =============================>>>>>>>>>>>\n");
			readPlayerAddNotify(res,res_len);
			break;
		}
		case ROOM_PLAYER_REMOVE:
		{
			fprintf(stderr, "[method: ROOM_PLAYER_REMOVE] =============================>>>>>>>>>>>\n");
			readPlayerRemoveNotify(res,res_len);
			break;
		}
		
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
		RoomLoginRes gameState;
		std::string state = currentState;
		std::string err;
		int ret = pbjson::json2pb(state, &gameState, err);
		fprintf(stderr, "[return old state: %s] item size: %d,ret:%d\n", currentState.c_str(), fres.items_size(),ret);
		for (int i = 0; i < fres.items_size(); i++) {
			const RoomLadderNotify::Item& item = fres.items(i);
			for( int j = 0 ; j < gameState.snap_shot().players_size() ; j++){
				RoomPlayer* player = gameState.mutable_snap_shot()->mutable_players(j);
				if(player->id() == item.id() && player->score() != item.score()){
					player->set_score(item.score());
					//player->set_score(1111);
				}
			}
			fprintf(stderr, "[method: %s] parse game result succ [ id: %d,score:%d,kill_times:%d,dead_times:%d ].\n", "p3", item.id(),item.score(),item.kill_times(),item.dead_times());
		}
		//update state
		std::string new_state;
		pbjson::pb2json(&gameState, new_state);
		currentState = new_state;
		//delete &gameState;
		fprintf(stderr, "[method: readLADDERNotify] parse game state:%s\n,new state:%s",state.c_str(),new_state.c_str());
		
		
	}
	return 0;
}

int ClientImpl::readDRAGNotify(void *res, uint32_t &res_len)
{	
	RoomPlayerDragRes fres;
	if(0 == res_len || !fres.ParseFromArray(res, res_len)){
		fprintf(stderr, "[method: %s] parse game result failed.\n", "readDRAGNotify");
	}else{
		RoomLoginRes gameState;
		std::string state = currentState;
		std::string err;
		int ret = pbjson::json2pb(state, &gameState, err);
		fprintf(stderr, "[return old state: %s] ret:%d\n", currentState.c_str(),ret);
		for( int j = 0 ; j < gameState.snap_shot().players_size() ; j++){
			RoomPlayer* player = gameState.mutable_snap_shot()->mutable_players(j);
			if(player->id() == gameState.id()){
					Vector *posvec1 = new  Vector();
					posvec1->set_x(fres.pos().x());
					posvec1->set_z(fres.pos().z());
					
					Vector *dirvec1 = new  Vector();
					dirvec1->set_x(fres.dir().x());
					dirvec1->set_z(fres.dir().z());
					
					player->set_allocated_pos(posvec1);
					player->set_allocated_dir(dirvec1);
			}
		}
			
		//update state
		std::string new_state;
		pbjson::pb2json(&gameState, new_state);
		currentState = new_state;
		
		fprintf(stderr, "[method: readDRAGNotify] parse game state:%s\n,new state:%s",state.c_str(),new_state.c_str());
	
		
		//end drag req
		uint32_t cmd = ROOM_PLAYER_DRAG_END;
		RoomPlayerDragEndReq dragEnd_;
		char dragreq[8192];
		if(!dragEnd_.SerializeToArray(dragreq, sizeof(dragreq))){
			fprintf(stderr, "game serialize req failed.\n");
		}
		uint32_t req_len = dragEnd_.GetCachedSize();
		doWriteRequest(cmd, dragreq, req_len);
		log_info("[method: readDRAGNotify] ==========================>>>>>>>>>drag end,cmd:%d,id:%d,x:%d,z:%d,dx:%d,dz:%d",cmd,gameState.id(),fres.pos().x(),fres.pos().z(),fres.dir().x(),fres.dir().z());
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
		fprintf(stderr, "[method: %s] item size: %d,frame:%d.\n", "readFrameNotify", fres.list_size(),frameid);
		for (int i = 0; i < fres.list_size(); i++) {
			const Frame& frame = fres.list(i);
			fprintf(stderr, "[method: %s] i:%d.\n","p2",i);
			int32_t cmd = frame.cmd();
			const ::std::string data = frame.data();
			readFrameData(cmd,data);
			fprintf(stderr, "[method: %s] parse game result succ [ cmd: %d ].\n", "readFrameNotify", cmd);
		}
		
	}
	return 0;
}

int ClientImpl::readFrameData(uint32_t cmd, const ::std::string &res)
{
	log_info("readFrameData [ cmd: %d ].\n", cmd);
	switch(cmd)
	{
		case CMD_MOVE:
		{	
			readFrameDataMove(res);
			break;
		}
		case CMD_SCENE_ITEM_CREATE:
		{	
			readSceneItemCreate(res);
			break;
		}
		case CMD_SCENE_TIEM_REMOVE:
		{	
			readSceneItemRemove(res);
			break;
		}
		case CMD_BASE_STATE:
		{	
			readSceneBaseState(res);
			break;
		}
		case CMD_LEVEL_UP:
		{	
			readLevelUp(res);
			break;
		}
		case CMD_DEATH:
		{	
			readDeath(res);
			break;
		}
		case CMD_SPELL_SATE:
		{	
			readFrameSpellLevel(res);
			break;
		}
		case CMD_BUFF_START:
		{	
			readBuffStart(res);
			break;
		}
		case CMD_BUFF_REMOVE:
		{	
			readBuffRemove(res);
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
		
		RoomLoginRes gameState;
		std::string state = currentState;
		std::string err;
		int ret = pbjson::json2pb(state, &gameState, err);
		fprintf(stderr, "[method: %s] parse game ret:%d,error:%s.\n", "readFrameDataMove",ret,err.c_str());
		
		for( int j = 0 ; j < gameState.snap_shot().players_size() ; j++){
			RoomPlayer* player = gameState.mutable_snap_shot()->mutable_players(j);
			if( player->id() == framemove.id() ){ //&& player->id() != gameState.id()){
				Vector *posvec = new  Vector();
				posvec->set_x(framemove.cur_pos().x());
				posvec->set_z(framemove.cur_pos().z());
				
				Vector *dirvec = new  Vector();
				dirvec->set_x(framemove.cur_dir().x());
				dirvec->set_z(framemove.cur_dir().z());
				
				player->set_allocated_pos(posvec);
				player->set_allocated_dir(dirvec);
			}
		}
		
		//update state
		std::string new_state;
		pbjson::pb2json(&gameState, new_state);
		currentState = new_state;
		fprintf(stderr, "[method: readFrameDataMove] parse game state:%s\n,new state:%s\n",state.c_str(),new_state.c_str());
		
	}
	fprintf(stderr, "[method: readFrameDat done................\n");
	return 0;
}

int ClientImpl::readSceneItemCreate(const ::std::string &res){
	fprintf(stderr, "read readSceneItemCreate.\n");
	FrameSceneItemCreate item;
	if(!item.ParseFromString(res)){
		fprintf(stderr, "[method: %s] parse game result failed.\n", "readSceneItemCreate");
	}else{
		RoomLoginRes gameState;
		std::string state = currentState;
		std::string err;
		int ret = pbjson::json2pb(state, &gameState, err);
		fprintf(stderr, "[method: %s] parse game ret:%d,error:%s.\n", "readSceneItemCreate",ret,err.c_str());
		
		RoomItem* roomitem = gameState.mutable_snap_shot()->add_items();
		roomitem->set_id(item.item().id());
		roomitem->set_xml_id(item.item().xml_id());
		
		Vector *posvec = new  Vector();
		posvec->set_x(item.item().pos().x());
		posvec->set_z(item.item().pos().z());
		roomitem->set_allocated_pos(posvec);
		//delete posvec;
		
		//update state
		std::string new_state;
		pbjson::pb2json(&gameState, new_state);
		currentState = new_state;
		fprintf(stderr, "[method: readSceneItemCreate] parse game state:%s\n,new state:%s\n",state.c_str(),new_state.c_str());
	}
	return 0;
}
int ClientImpl::readSceneItemRemove(const ::std::string &res){
	fprintf(stderr, "read readSceneItemRemove.\n");
	FrameSceneItemRemove item;
	if(!item.ParseFromString(res)){
		fprintf(stderr, "[method: %s] parse game result failed.\n", "readSceneItemRemove");
	}else{
		RoomLoginRes gameState;
		std::string state = currentState;
		std::string err;
		int ret = pbjson::json2pb(state, &gameState, err);
		fprintf(stderr, "[method: %s] parse game ret:%d,error:%s.\n", "readSceneItemRemove",ret,err.c_str());
		
		for( int j = 0 ; j < gameState.snap_shot().items_size() ; j++){
			RoomItem* roomItem = gameState.mutable_snap_shot()->mutable_items(j);
			if(roomItem->id() == item.id() ){
				log_info("[method: readSceneItemRemove] remove id:%d/n",roomItem->id());
				roomItem->Clear();
				//DeleteElementsFromRepeatedField(*roomItem, gameState.mutable_snap_shot()->mutable_items);
				const google::protobuf::Descriptor  *descriptor = gameState.mutable_snap_shot()->GetDescriptor();
				const google::protobuf::Reflection  *reflection = gameState.mutable_snap_shot()->GetReflection();
				const google::protobuf::FieldDescriptor* field = descriptor->FindFieldByName("items");
				RemoveFromRepeatedField(reflection,field,gameState.mutable_snap_shot(),j,gameState.snap_shot().items_size());
			}
		}
		
		
		//update state
		std::string new_state;
		pbjson::pb2json(&gameState, new_state);
		currentState = new_state;
		fprintf(stderr, "[method: readSceneItemRemove] parse game state:%s\n,new state:%s\n",state.c_str(),new_state.c_str());
	}
	return 0;
}

//buff
int ClientImpl::readBuffStart(const ::std::string &res){
	fprintf(stderr, "read readBuffStart.\n");
	FrameStartBuff buff;
	if(!buff.ParseFromString(res)){
		fprintf(stderr, "[method: %s] parse game result failed.\n", "readBuffStart");
	}else{
		RoomLoginRes gameState;
		std::string state = currentState;
		std::string err;
		int ret = pbjson::json2pb(state, &gameState, err);
		fprintf(stderr, "[method: %s] parse game ret:%d,error:%s.\n", "readBuffStart",ret,err.c_str());
		
		RoomBuff* roomitem = gameState.mutable_snap_shot()->add_buffs();
		roomitem->set_id(buff.buff().id());
		roomitem->set_xml_id(buff.buff().xml_id());
		roomitem->set_player_id(buff.buff().player_id());
		roomitem->set_remain_time(buff.buff().remain_time());
		
		Vector *posvec = new  Vector();
		posvec->set_x(buff.buff().pos().x());
		posvec->set_z(buff.buff().pos().z());
		roomitem->set_allocated_pos(posvec);
		
		Vector *dirvec = new  Vector();
		dirvec->set_x(buff.buff().dir().x());
		dirvec->set_z(buff.buff().dir().z());
		roomitem->set_allocated_dir(dirvec);

		//update state
		std::string new_state;
		pbjson::pb2json(&gameState, new_state);
		currentState = new_state;
		fprintf(stderr, "[method: readBuffStart] parse game state:%s\n,new state:%s\n",state.c_str(),new_state.c_str());
	}
	return 0;
}
int ClientImpl::readBuffRemove(const ::std::string &res){
	fprintf(stderr, "read readBuffRemove.\n");
	FrameRemoveBuff buff;
	if(!buff.ParseFromString(res)){
		fprintf(stderr, "[method: %s] parse game result failed.\n", "readBuffRemove");
	}else{
		RoomLoginRes gameState;
		std::string state = currentState;
		std::string err;
		int ret = pbjson::json2pb(state, &gameState, err);
		fprintf(stderr, "[method: %s] parse game ret:%d,error:%s.\n", "readBuffRemove",ret,err.c_str());
		
		for( int j = 0 ; j < gameState.snap_shot().buffs_size() ; j++){
			RoomBuff* roomItem = gameState.mutable_snap_shot()->mutable_buffs(j);
			if(roomItem->id() == buff.buff_id() ){
				log_info("[method: readBuffRemove] remove id:%s/n",roomItem->id().c_str());
				roomItem->Clear();
				//DeleteElementsFromRepeatedField(*roomItem, gameState.mutable_snap_shot()->mutable_items);
				const google::protobuf::Descriptor  *descriptor = gameState.mutable_snap_shot()->GetDescriptor();
				const google::protobuf::Reflection  *reflection = gameState.mutable_snap_shot()->GetReflection();
				const google::protobuf::FieldDescriptor* field = descriptor->FindFieldByName("buffs");
				RemoveFromRepeatedField(reflection,field,gameState.mutable_snap_shot(),j,gameState.snap_shot().buffs_size());
			}
		}
		
		
		//update state
		std::string new_state;
		pbjson::pb2json(&gameState, new_state);
		currentState = new_state;
		fprintf(stderr, "[method: readSceneItemRemove] parse game state:%s\n,new state:%s\n",state.c_str(),new_state.c_str());
	}
	return 0;
}

//player
int ClientImpl::readPlayerAddNotify(void *res, uint32_t &res_len){
	fprintf(stderr, "read readPlayerAddNotify.\n");
	RoomPlayerAdd player;
	if(0 == res_len || !player.ParseFromArray(res, res_len)){
		fprintf(stderr, "[method: %s] parse game result failed.\n", "readPlayerAddNotify");
	}else{
		RoomLoginRes gameState;
		std::string state = currentState;
		std::string err;
		int ret = pbjson::json2pb(state, &gameState, err);
		fprintf(stderr, "[method: %s] parse game ret:%d,error:%s.\n", "readPlayerAddNotify",ret,err.c_str());
		
		RoomPlayer* roomitem = gameState.mutable_snap_shot()->add_players();
		roomitem->set_id(player.player().id());
		roomitem->set_nickname(player.player().nickname());
		roomitem->set_job(player.player().job());
		roomitem->set_level(player.player().level());
		roomitem->set_score(player.player().score());
		
		Vector *posvec = new  Vector();
		posvec->set_x(player.player().pos().x());
		posvec->set_z(player.player().pos().z());
		roomitem->set_allocated_pos(posvec);
		
		Vector *dirvec = new  Vector();
		dirvec->set_x(player.player().dir().x());
		dirvec->set_z(player.player().dir().z());
		roomitem->set_allocated_dir(dirvec);
		
		RoomPlayerState *playerState = new  RoomPlayerState();
		playerState->set_hp(player.player().state().hp());
		playerState->set_speed(player.player().state().speed());
		roomitem->set_allocated_state(playerState);
		
		RoomPlayerSpell* spellItem = roomitem->add_spells();
		for(int i = 0 ; i < player.player().spells_size() ; i++){
			spellItem->set_xml_id(player.player().spells(i).xml_id());
			spellItem->set_level(player.player().spells(i).level());
			spellItem->set_cd(player.player().spells(i).cd());
			log_info("[method: readPlayerAddNotify] add player spell succ,xml id:%d",player.player().spells(i).xml_id());
		}
		log_info("[method: readPlayerAddNotify] update spell succ,playerid:%d,",player.player().id());
		//update state
		std::string new_state;
		pbjson::pb2json(&gameState, new_state);
		currentState = new_state;
		fprintf(stderr, "[method: readPlayerAddNotify] parse game state:%s\n,new state:%s\n",state.c_str(),new_state.c_str());
	}
	return 0;
}
int ClientImpl::readPlayerRemoveNotify(void *res, uint32_t &res_len){
	fprintf(stderr, "read readBuffRemove.\n");
	RoomPlayerRemove player;
	if(0 == res_len || !player.ParseFromArray(res, res_len)){
		fprintf(stderr, "[method: %s] parse game result failed.\n", "readBuffRemove");
	}else{
		RoomLoginRes gameState;
		std::string state = currentState;
		std::string err;
		int ret = pbjson::json2pb(state, &gameState, err);
		fprintf(stderr, "[method: %s] parse game ret:%d,error:%s.\n", "readBuffRemove",ret,err.c_str());
		
		for( int j = 0 ; j < gameState.snap_shot().players_size() ; j++){
			RoomPlayer* roomItem = gameState.mutable_snap_shot()->mutable_players(j);
			if(roomItem->id() == player.id() ){
				log_info("[method: readBuffRemove] remove id:%d/n",roomItem->id());
				roomItem->Clear();
				const google::protobuf::Descriptor  *descriptor = gameState.mutable_snap_shot()->GetDescriptor();
				const google::protobuf::Reflection  *reflection = gameState.mutable_snap_shot()->GetReflection();
				const google::protobuf::FieldDescriptor* field = descriptor->FindFieldByName("players");
				RemoveFromRepeatedField(reflection,field,gameState.mutable_snap_shot(),j,gameState.snap_shot().players_size());
			}
		}
		
		
		//update state
		std::string new_state;
		pbjson::pb2json(&gameState, new_state);
		currentState = new_state;
		fprintf(stderr, "[method: readSceneItemRemove] parse game state:%s\n,new state:%s\n",state.c_str(),new_state.c_str());
	}
	return 0;
}
//end

int ClientImpl::readSceneBaseState(const ::std::string &res){
	fprintf(stderr, "read readBaseState.\n");
	FramePlayerBaseState baseState;
	if(!baseState.ParseFromString(res)){
		fprintf(stderr, "[method: %s] parse game result failed.\n", "readBaseState");
	}else{
		RoomLoginRes gameState;
		std::string state = currentState;
		std::string err;
		int ret = pbjson::json2pb(state, &gameState, err);
		fprintf(stderr, "[method: %s] parse game ret:%d,error:%s.\n", "readBaseState",ret,err.c_str());
		
		for( int j = 0 ; j < gameState.snap_shot().players_size() ; j++){
			RoomPlayer* player = gameState.mutable_snap_shot()->mutable_players(j);
			if( player->id() == baseState.player_id()){
				RoomPlayerState *playerState = new  RoomPlayerState();
				playerState->set_hp(baseState.state().hp());
				playerState->set_speed(baseState.state().speed());
				
				player->set_allocated_state(playerState);
				//delete playerState;
			}
		}
		
		//update state
		std::string new_state;
		pbjson::pb2json(&gameState, new_state);
		currentState = new_state;
		fprintf(stderr, "[method: readBaseState] parse game state:%s\n,new state:%s\n",state.c_str(),new_state.c_str());
	}
	return 0;
}

int ClientImpl::readLevelUp(const ::std::string &res){
	fprintf(stderr, "read readLevelUp.\n");
	FrameLevelUp levelUp;
	if(!levelUp.ParseFromString(res)){
		fprintf(stderr, "[method: %s] parse game result failed.\n", "readLevelUp");
	}else{
		RoomLoginRes gameState;
		std::string state = currentState;
		std::string err;
		int ret = pbjson::json2pb(state, &gameState, err);
		fprintf(stderr, "[method: %s] parse game ret:%d,error:%s.\n", "readLevelUp",ret,err.c_str());
		
		for( int j = 0 ; j < gameState.snap_shot().players_size() ; j++){
			RoomPlayer* player = gameState.mutable_snap_shot()->mutable_players(j);
			if( player->id() == levelUp.player_id()){
				player->set_level(levelUp.player_level());
				log_info("[method: readLevelUp] set player id:%d.",player->id());
			}
		}
		
		//update state
		std::string new_state;
		pbjson::pb2json(&gameState, new_state);
		currentState = new_state;
		fprintf(stderr, "[method: readLevelUp] parse game state:%s\n,new state:%s\n",state.c_str(),new_state.c_str());
	}
	return 0;
}

int ClientImpl::readDeath(const ::std::string &res){
	fprintf(stderr, "read readDeath.\n");
	FrameDeath death;
	if(!death.ParseFromString(res)){
		fprintf(stderr, "[method: %s] parse game result failed.\n", "readDeath");
	}else{
		RoomLoginRes gameState;
		std::string state = currentState;
		std::string err;
		int ret = pbjson::json2pb(state, &gameState, err);
		fprintf(stderr, "[method: %s] parse game ret:%d,error:%s.\n", "readDeath",ret,err.c_str());
		
		for( int j = 0 ; j < gameState.snap_shot().players_size() ; j++){
			RoomPlayer* player = gameState.mutable_snap_shot()->mutable_players(j);
			if( player->id() == death.id()){
				RoomPlayerState *playerState = new  RoomPlayerState();
				playerState->set_hp(0);
				playerState->set_speed(player->state().speed());
				log_info("[method: readDeath] set dead player id:%d",player->id());
				player->set_allocated_state(playerState);
			}
		}
		//update state
		std::string new_state;
		pbjson::pb2json(&gameState, new_state);
		currentState = new_state;
		fprintf(stderr, "[method: readDeath] parse game state:%s\n,new state:%s\n",state.c_str(),new_state.c_str());
	}
	return 0;
}


int ClientImpl::readFrameSpellLevel(const ::std::string &res){
	fprintf(stderr, "read readFrameSpellLevel.\n");
	FrameSpellLevel spell;
	if(!spell.ParseFromString(res)){
		fprintf(stderr, "[method: %s] parse game result failed.\n", "readFrameSpellLevel");
	}else{
		RoomLoginRes gameState;
		std::string state = currentState;
		std::string err;
		int ret = pbjson::json2pb(state, &gameState, err);
		fprintf(stderr, "[method: %s] parse game ret:%d,error:%s.\n", "readFrameSpellLevel",ret,err.c_str());
		
		for( int j = 0 ; j < gameState.snap_shot().players_size() ; j++){
			RoomPlayer* player = gameState.mutable_snap_shot()->mutable_players(j);
			if( player->id() == spell.player_id()){
				for(int i = 0 ; i < player->spells_size() ; i++){
					for(int k = 0 ; k < spell.spells_size() ; k++){
						if(player->mutable_spells(i)->xml_id() == spell.mutable_spells(k)->xml_id() ){
							player->mutable_spells(i)->set_level(spell.mutable_spells(k)->level());
							player->mutable_spells(i)->set_cd(spell.mutable_spells(k)->cd());
							log_info("[method: readFrameSpellLevel] update spell succ,playerid:%d,xml id:%d",player->id(),spell.mutable_spells(k)->xml_id());
						}
					}
				}
			}
		}
		//update state
		std::string new_state;
		pbjson::pb2json(&gameState, new_state);
		currentState = new_state;
		fprintf(stderr, "[method: readFrameSpellLevel] parse game state:%s\n,new state:%s\n",state.c_str(),new_state.c_str());
	}
	return 0;
}


void ClientImpl::RemoveFromRepeatedField(
    const google::protobuf::Reflection *reflection,
    const google::protobuf::FieldDescriptor *field,
    google::protobuf::Message *message,
    int row,
    int count)
{
    int size = reflection->FieldSize(*message, field);
    // shift all remaining elements
    for (int i = row; i < size - count; ++i)
        reflection->SwapElements(message, field, i, i + count);
    // delete elements from reflection
    for (int i = 0; i < count; ++i)
        reflection->RemoveLast(message, field);
}


int ClientImpl::setGameState(RoomLoginRes *currentState)
{
		gameState1 = currentState;
		return 1;
}

RoomLoginRes* ClientImpl::getGameState(){
	return gameState1;
}


}; // namespace ai
