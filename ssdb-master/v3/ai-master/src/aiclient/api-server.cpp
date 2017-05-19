 
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <string.h>
#include <ctime>
#include <unistd.h>
#include "AI_client.h"
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
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	const char *ip = (argc >= 2)? argv[1] : "192.168.89.42";
	int port = (argc >= 3)? atoi(argv[2]) : 19201;
	
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
