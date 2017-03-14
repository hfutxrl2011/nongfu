/**
* @file ssdb-server.cpp
* @Brief entry file of server
* @author Youzu-BigApp
* @version 1.0.0
* @date 2016-03-10
*/
#include "include.h"
#include "version.h"
#include "net/server.h"
#include "util/app.h"
#include "util/log.h"
#include "util/config.h"
#include "util/exception.h"
#include "util/tools.h"
#include "util/thread_mem.h"
#include "serv.h"
#include <google/protobuf/stubs/common.h>

#define APP_NAME "ssdb-server"
#define APP_VERSION SSDB_VERSION

class MyApplication : public Application
{
public:
	virtual void usage(int argc, char **argv);
	virtual void welcome();
	virtual void run();
};

void MyApplication::welcome(){
	fprintf(stderr, "%s\n", "Product:\tYouzu-BigApp");
	fprintf(stderr, "Module:\t\t%s\n", APP_NAME);
	fprintf(stderr, "Version:\t%s\n", APP_VERSION);
	fprintf(stderr, "Build Date:\t%s %s\n", __DATE__, __TIME__);
	fprintf(stderr, "Copyright (c) 2016-2020 by vijay\n");
}

void MyApplication::usage(int argc, char **argv){
	printf("Usage:\n");
	printf("    %s [-d] -f /path/to/ssdb.conf [-s start|stop|restart]\n", argv[0]);
	printf("Options:\n");
	printf("    -f    path of conf\n");
	printf("    -d    run as daemon\n");
	printf("    -s    option to start|stop|restart the server\n");
	printf("    -h    show this message\n");
}

void MyApplication::run(){
	log_info("ssdb-server %s", APP_VERSION);
	log_info("conf_file        : %s", app_args.conf_file.c_str());
	log_info("log_level        : %s", Logger::shared()->level_name().c_str());
	log_info("log_output       : %s", Logger::shared()->output_name().c_str());
	log_info("log_rotate_size  : %" PRId64, Logger::shared()->rotate_size());

	NetworkServer *net = NULL;	
	SSDBServer *server = NULL;
	net = NetworkServer::init(*conf);
	ASSERT_SYS(NULL != net, "try to new NetworkServer failed, no memory");
	server = new(std::nothrow) SSDBServer(*conf, net);
	ASSERT_SYS(NULL != server, "try to new SSDBServer failed, no memory");
	
	log_info("pidfile: %s, pid: %d", app_args.pidfile.c_str(), (int)getpid());
	log_info("ssdb server started.");
	net->serve();
	SAFE_DELETE(net);
	SAFE_DELETE(server);

	log_info("%s exit.", APP_NAME);
}

int main(int argc, char **argv){
	FUNC_START
		GOOGLE_PROTOBUF_VERIFY_VERSION;	
		MyApplication app;
		app.main(argc, argv);
	FUNC_END(log_info, log_fatal)
	google::protobuf::ShutdownProtobufLibrary(); 
	int ret = 0;
	FUNC_GET_ERROR(ret);	
	return SSDB_E_OK == ret ? 0 : -1;
}
