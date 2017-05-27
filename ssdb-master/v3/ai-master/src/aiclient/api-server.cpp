 
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <string.h>
#include <ctime>
#include <unistd.h>
#include "AI_client.h"
#include "../util/log.h"
#include "game.pb.h"
#include "actions.pb.h"
#include "pbjson.hpp"
#include "cepollclient.h"


using namespace ai;
using namespace pb;
using namespace std;
using namespace google::protobuf;
using namespace pbjson;

int main(int argc, char **argv){
	
	{ // logger
		std::string log_output;
		std::string log_level_;
		std::string log_output_dir;
		int64_t log_rotate_size;

		log_level_ = "frame";
		//strtolower(&log_level_);
		if(log_level_.empty()){
			log_level_ = "debug";
		}
		int level = Logger::get_level(log_level_.c_str());
		log_rotate_size = 1000000000;
		log_output = "log.txt";
		if(log_output == ""){
			log_output = "stdout";
		}
		log_output_dir = "log";
		if(log_output_dir == ""){
			log_output_dir = "./";
		}
		if(log_open(log_output.c_str(), level, true, log_rotate_size, log_output_dir.c_str()) == -1){
			fprintf(stderr, "error opening log file: %s\n", log_output.c_str());
			exit(1);
		}
	}
	
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	const char *ip = (argc >= 2)? argv[1] : "192.168.1.103";//"192.168.89.42";
	int port = (argc >= 3)? atoi(argv[2]) : 19201;
	
	
	log_info("run api server ..........");
	
	CEpollClient *pCEpollClient = new CEpollClient(1, ip, port);
	if(NULL == pCEpollClient){
		cout<<"[epollclient error]:main init"<<"Init CEpollClient fail"<<endl;
	}
	pCEpollClient->RunFun();
	if(NULL != pCEpollClient){
		delete pCEpollClient;
		pCEpollClient = NULL;
	}
	google::protobuf::ShutdownProtobufLibrary(); 
	return 0;
}
