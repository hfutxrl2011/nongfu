 
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <string.h>
#include <ctime>
#include <unistd.h>
#include "AI_client.h"
#include "game.pb.h"

using namespace ai;
using namespace pb;
using namespace std;

int main(int argc, char **argv){
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	//192.168.89.42:9201
	const char *ip = (argc >= 2)? argv[1] : "192.168.89.42";
	int port = (argc >= 3)? atoi(argv[2]) : 9201;

	Client *client = Client::connect(ip, port);
	if(client == NULL){
		printf("fail to connect to server!\n");
		google::protobuf::ShutdownProtobufLibrary(); 
		return 0;
	}
	//game start============================
	
	RoomLoginReq req_;
	req_.set_nickname("AIagent");
	req_.set_room_id(123);
	req_.set_room_key("keys");
	req_.set_job(1);
	
	char req[8192];
	if(!req_.SerializeToArray(req, sizeof(req))){
		fprintf(stderr, "game serialize req failed.\n");
		return 0;
	}

	uint32_t req_len = req_.GetCachedSize();

	char res[8192];
	uint32_t res_len = sizeof(res);

	memset(res, 0x00, sizeof(res));
	int ret = client->roomlogin(req, req_len, res, res_len);

	if(0 == ret){
		RoomLoginRes fres;
		if(0 == res_len || !fres.ParseFromArray(res, res_len)){
			fprintf(stderr, "[method: %s] parse game result failed.\n", "get");
		}else{
			int32_t error_code = fres.id();
			//const char *error_info = fres.game_issue().c_str();
			//const char *return_data = fres.game_issue().c_str();
			
			
			//std::string str;
			//fres.SerializeToString(&str);
			//fprintf(stderr, "[method: %s] parse game result succ [ error_code: %d, "
			//		"error_info: %s, returned_data: %s ].\n", str, error_code, error_info, return_data);
			
			
			//RoomSnapShot snap;
			fprintf(stderr, "[method: %s] player size: %d", "p1", fres.snap_shot().players_size());
			for (int i = 0; i < fres.snap_shot().players_size(); i++) {
				const RoomPlayer& player = fres.snap_shot().players(i);
				fprintf(stderr, "[method: %s] i:%d","p2",i);
				int32_t uid = player.id();
				const char *nickname = player.nickname().c_str();
				Vector pos = player.pos();
				
				for (int i = 0; i < player.spells_size(); i++) {
					const RoomPlayerSpell& spell = player.spells(i);
					fprintf(stderr, "[method: %s] i:%d,xml_id:%d,cd:%d,level:%d","p22",i,spell.xml_id(),spell.cd(),spell.level());
				}
				fprintf(stderr, "[method: %s] parse game result succ [ error_code: %d, "
					"nickname: %s ,x:%d,z:%d].\n", "get1", uid, nickname,player.pos().x(),player.pos().z());
			}
			
			fprintf(stderr, "[method: %s] item size: %d", "p1", fres.snap_shot().items_size());
			for (int i = 0; i < fres.snap_shot().items_size(); i++) {
				const RoomItem& items = fres.snap_shot().items(i);
				fprintf(stderr, "[method: %s] i:%d","p3",i);
				int32_t itemid = items.id();
				Vector pos = items.pos();
				fprintf(stderr, "[method: %s] parse game result succ [ error_code: %d, "
					"xml_id: %d ,x:%d,z:%d].\n", "get2", itemid, items.xml_id(),pos.x(),pos.z());
			}
			
			fprintf(stderr, "[method: %s] buffs size: %d", "p1", fres.snap_shot().buffs_size());
			for (int i = 0; i < fres.snap_shot().buffs_size(); i++) {
				const RoomBuff& buffs = fres.snap_shot().buffs(i);
				fprintf(stderr, "[method: %s] i:%d","p4",i);
				const char * bid = buffs.id().c_str();
				Vector pos = buffs.pos();
				fprintf(stderr, "[method: %s] parse game result succ [ error_code: %s, "
					"xml_id: %d ,remain_time:%d,x:%d,z:%d].\n", "get3", bid, buffs.xml_id(),buffs.remain_time(),pos.x(),pos.z());
			}
			
		}
	}
	
	//move
	RoomPlayerMoveReq movereq_;
	Vector *posvec = new  Vector();
	posvec->set_x(2000);
	posvec->set_z(1000);
	movereq_.set_allocated_pos(posvec);
	
	Vector *dirvec = new  Vector();
	dirvec->set_x(10);
	dirvec->set_z(10);
	movereq_.set_allocated_dir(dirvec);
	
	char movereq[8192];
	if(!movereq_.SerializeToArray(movereq, sizeof(movereq))){
		fprintf(stderr, "game serialize req failed.\n");
		return 0;
	}
	
	uint32_t move_req_len = movereq_.GetCachedSize();

	char moveres[8096];
	uint32_t move_res_len = sizeof(moveres);
	
	int moveret = client->reqmove(movereq, move_req_len, moveres, move_res_len);

	if(0 == moveret){
		RoomPlayerDragRes mres;
		if(0 == move_res_len || !mres.ParseFromArray(moveres, move_res_len)){
			fprintf(stderr, "[method: %s] parse game result failed.\n", "move");
		}else{
			Vector res_pos = mres.pos();
			fprintf(stderr, "[method: %s] parse game x:%d,z:%d.\n", "move",res_pos.x(),res_pos.z());
		}
	}else{
		fprintf(stderr, "[method: %s] parse game result failed.\n", "movereq");
	}
	
	//wait
	//int32_t time = 30;
	fprintf(stderr, "parse game notice start.[notice cmd : %d]\n", 0);
	do{
		uint32_t notice_cmd = 0;
		char noticeres[8096];
		uint32_t notice_res_len = sizeof(noticeres);
		int notice_ret = client->readNotify(notice_cmd, noticeres, notice_res_len);
		fprintf(stderr, "[method: p1] parse game notice succ.[notice cmd : %u]\n", notice_cmd);
		if(0 == notice_ret){
			fprintf(stderr, "[method: p2] parse game notice succ.[notice cmd : %u]\n", notice_cmd);
		}
		
		usleep(3000 * 1000);
		//time--;
	}while(1);
	
	//game end===============================
	
	delete client;
	google::protobuf::ShutdownProtobufLibrary(); 
	return 0;
}
