 
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <string.h>
#include "AI_client.h"
#include "../request_example.pb.h"
#include "../response_example.pb.h"
#include "game.pb.h"

using namespace ai;
using namespace proto;

int main(int argc, char **argv){
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	const char *ip = (argc >= 2)? argv[1] : "127.0.0.1";
	int port = (argc >= 3)? atoi(argv[2]) : 8063;

	Client *client = Client::connect(ip, port);
	if(client == NULL){
		printf("fail to connect to server!\n");
		google::protobuf::ShutdownProtobufLibrary(); 
		return 0;
	}

/*
	request_example_req_t req_;
	Person *person = req_.add_people();
	person->set_id(123456);
	*person->mutable_name() = "wu_jing";
	person->set_email("wu_jing@baidu.com");
	Person::PhoneNumber *phone_number = person->add_phones();
	phone_number->set_number("18201929808");
	phone_number->set_type(Person::MOBILE);
	phone_number->set_number("18201929809");
	phone_number->set_type(Person::WORK);
	
	char req[8192];
	if(!req_.SerializeToArray(req, sizeof(req))){
		fprintf(stderr, "serialize req failed.\n");
		return 0;
	}
	uint32_t req_len = req_.GetCachedSize();

	char res[8192];
	uint32_t res_len = sizeof(res);

	memset(res, 0x00, sizeof(res));
	int ret = client->get(req, req_len, res, res_len);
	if(0 == ret){
		request_example_res_t fres;
		if(0 == res_len || !fres.ParseFromArray(res, res_len)){
			fprintf(stderr, "[method: %s] parse result failed.\n", "get");
		}else{
			int32_t error_code = fres.error_code();
			const char *error_info = fres.error_info().c_str();
			const char *return_data = fres.return_data().c_str();
			fprintf(stderr, "[method: %s] parse result succ [ error_code: %d, "
					"error_info: %s, returned_data: %s ].\n", "get", error_code, error_info, return_data);
		}
	}
	res_len = sizeof(res);
	memset(res, 0x00, sizeof(res));
	ret = client->set(req, req_len, res, res_len);
	if(0 == ret){
		request_example_res_t fres;
		if(0 == res_len || !fres.ParseFromArray(res, res_len)){
			fprintf(stderr, "[method: %s] parse result failed.\n", "set");
		}else{
			int32_t error_code = fres.error_code();
			const char *error_info = fres.error_info().c_str();
			const char *return_data = fres.return_data().c_str();
			fprintf(stderr, "[method: %s] parse result succ [ error_code: %d, "
					"error_info: %s, returned_data: %s ].\n", "set", error_code, error_info, return_data);
		}
	}

*/

	//game start============================
	
	LoginRes req_;
	req_.set_id(345678);
	req_.set_name("name");
	req_.set_score(123);
	
	char req[8192];
	if(!req_.SerializeToArray(req, sizeof(req))){
		fprintf(stderr, "game serialize req failed.\n");
		return 0;
	}

	uint32_t req_len = req_.GetCachedSize();

	char res[8192];
	uint32_t res_len = sizeof(res);

	memset(res, 0x00, sizeof(res));
	int ret = client->getgame(req, req_len, res, res_len);

	if(0 == ret){
		request_example_res_t fres;
		if(0 == res_len || !fres.ParseFromArray(res, res_len)){
			fprintf(stderr, "[method: %s] parse game result failed.\n", "get");
		}else{
			int32_t error_code = fres.error_code();
			const char *error_info = fres.error_info().c_str();
			const char *return_data = fres.return_data().c_str();
			fprintf(stderr, "[method: %s] parse game result succ [ error_code: %d, "
					"error_info: %s, returned_data: %s ].\n", "get", error_code, error_info, return_data);
		}
	}
	//game end===============================
	
	delete client;
	google::protobuf::ShutdownProtobufLibrary(); 
	return 0;
}
