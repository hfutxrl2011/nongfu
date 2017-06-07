#include <Python.h>
#include "cepollclient.h"
#include "util/log.h"

class Mytask : public Task{
public:
    Mytask(){}
	Client* client;
	
    int run(){
        log_info("thread[%lu] : %s\n",pthread_self(),(char*)this->arg_);
        runModel((char*)this->arg_);
        return 0;
    }
	
	bool runModel(std::string state)
	{
		bool bret = false;
		struct timeval tv;
		gettimeofday(&tv, NULL);
		if(tv.tv_usec % 100 >= 90){
			log_info("debug:==============> step2.1,return false,usec:%d",tv.tv_usec);
			return bret;
		}
		log_info("debug:==============> step2.1,usec:%d",tv.tv_usec);
		
		if ( !Py_IsInitialized() ){Py_Initialize();}
		if ( !Py_IsInitialized() ){return bret;}
		
		PyRun_SimpleString("import sys");
		PyRun_SimpleString("sys.path.append('./')");
		//PyRun_SimpleString("sys.path.append('./ai_pacman/igame/')");
		PyObject *pName,*pModule,*pDict,*pFunc;
		
		//pName = PyString_FromString("modelsvrtest");
		pName = PyString_FromString("action");
		pModule = PyImport_Import(pName);
		if ( !pModule ){
			printf("can't find modelsvrtest.py");
			return bret;
		}
		
		pDict = PyModule_GetDict(pModule);
		if ( !pDict ){
			return bret;
		}
		log_info("debug:==============> step2.2");
		//pFunc = PyDict_GetItemString(pDict, "run");
		pFunc = PyDict_GetItemString(pDict, "Action");
		if ( !pFunc || !PyCallable_Check(pFunc) )
		{
			printf("can't find function [run]");
			getchar();
			return bret;
		}
		
		PyObject* pParm = PyTuple_New(1);
		PyTuple_SetItem(pParm, 0, Py_BuildValue("s",state.c_str()));
		log_info("debug:==============> step2.3");
		PyObject* ret =PyObject_CallObject(pFunc, pParm);
		log_info("debug:==============> step2.4");
		log_info("do model,state0:%s",state.c_str());
		char* result = NULL;
		if (ret != NULL && PyString_Check(ret)){
			result = PyString_AsString(ret);
		}else{
			log_info("debug:=================>PyString_Check failed...");
			return bret;
		}
		log_info("debug:==============> step2.4.1,result:%s",result);
		//char *res = NULL;
		//strncpy(res, result, 512);
		cout <<"===========================>"<<endl;
		log_info("debug:==============> step2.5");
		
		bret = doAction(result);
		
		log_info("debug:==============> step2.6");
		log_info("do model,state:%s\n actions:%s",state.c_str(),result);
		
		Py_DECREF(pFunc);
		Py_DECREF(ret);
		Py_DECREF(pParm);
		Py_DECREF(pName);
		Py_DECREF(pModule);
		log_info("debug:==============> step2.7");
		Py_Finalize();
		log_info("debug:==============> step2.8");
		return bret;
		
	}

	bool doAction(char* result)
	{
		bool bret = false;
		char action[8096];//todo
		sprintf(action, "{\"actionitem\":%s}", result);
		printf("action:%s\n", action);
		
		ActionList actionlist;
		std::string err;
		int ret = pbjson::json2pb(action, &actionlist, err);
		if(0 == ret){
			bret = true;
		}
		printf("actionlist size:\%d\n", actionlist.actionitem_size());
		for (int i = 0; i < actionlist.actionitem_size(); i++) {
			const Action& action = actionlist.actionitem(i);
			if(action.action().compare("move") == 0){
				printf("move...\n");
				RoomPlayerMoveReq movereq_;
				Vector *posvec = new  Vector();
				posvec->set_x(action.pos().x());
				posvec->set_z(action.pos().z());
				movereq_.set_allocated_pos(posvec);
				
				Vector *dirvec = new  Vector();
				dirvec->set_x(action.dir().x());
				dirvec->set_z(action.dir().z());
				movereq_.set_allocated_dir(dirvec);
				
				char movereq[8192];
				if(!movereq_.SerializeToArray(movereq, sizeof(movereq))){
					fprintf(stderr, "game serialize req failed.\n");
				}
				uint32_t move_req_len = movereq_.GetCachedSize();
				client->reqmove(movereq, move_req_len);
				fprintf(stderr, "game move succ.\n");
				
				struct timeval tv;
				gettimeofday(&tv, NULL);
				if(tv.tv_usec % 100 >= 90){
					log_info("debug:==============> move update location by model,usec",tv.tv_usec);
					RoomLoginRes gameState;
					std::string state = client->currentState;
					std::string err;
					int ret = pbjson::json2pb(state, &gameState, err);
					fprintf(stderr, "[method: %s] parse game ret:%d,error:%s.\n", "do move action",ret,err.c_str());
						for( int j = 0 ; j < gameState.snap_shot().players_size() ; j++){
						RoomPlayer* player = gameState.mutable_snap_shot()->mutable_players(j);
						if(player->id() == gameState.id()){
							Vector *posvec1 = new  Vector();
							posvec1->set_x(action.pos().x());
							posvec1->set_z(action.pos().z());
							
							Vector *dirvec1 = new  Vector();
							dirvec1->set_x(action.dir().x());
							dirvec1->set_z(action.dir().z());
							
							player->set_allocated_pos(posvec1);
							player->set_allocated_dir(dirvec1);
						}
					}
					
					//update state
					std::string new_state;
					pbjson::pb2json(&gameState, new_state);
					client->currentState = new_state;
				}
				
			}else if(action.action().compare("stop") == 0){// not allow
				printf("stop...\n");
				/*
				RoomPlayerStopReq stopreq_;
				Vector *posvec1 = new  Vector();
				posvec1->set_x(action.pos().x());
				posvec1->set_z(action.pos().z());
				stopreq_.set_allocated_pos(posvec1);
				
				Vector *dirvec1 = new  Vector();
				//dirvec1->set_x(action.dir().x());
				//dirvec1->set_z(action.dir().z());
				dirvec1->set_x(0);
				dirvec1->set_z(0);
				stopreq_.set_allocated_dir(dirvec1);
				
				char stopreq[8192];
				if(!stopreq_.SerializeToArray(stopreq, sizeof(stopreq))){
					fprintf(stderr, "game serialize req failed.\n");
				}
				uint32_t stop_req_len = stopreq_.GetCachedSize();
				client->reqstop(stopreq, stop_req_len);
				*/
				fprintf(stderr, "game stop succ.\n");
			}else if(action.action().compare("spell") == 0){
				SpellStartReq spellreq_;
				spellreq_.set_xml_id(action.xml_id());
				Vector *posvec2 = new  Vector();
				posvec2->set_x(action.dir().x());
				posvec2->set_z(action.dir().z());
				spellreq_.set_allocated_pos(posvec2);
				
				Vector *dirvec2 = new  Vector();
				dirvec2->set_x(action.dir().x());
				dirvec2->set_z(action.dir().z());
				spellreq_.set_allocated_dir(dirvec2);
				
				char spellreq[8192];
				if(!spellreq_.SerializeToArray(spellreq, sizeof(spellreq))){
					fprintf(stderr, "game serialize req failed.\n");
				}
				uint32_t spell_req_len = spellreq_.GetCachedSize();
				client->reqreleasespell(spellreq, spell_req_len);
				printf("spell...\n");
			}else if(action.action().compare("relive") == 0){
				RoomPlayerReliveReq relivereq_;
				relivereq_.set_job(action.job());
				char relivereq[8192];
				if(!relivereq_.SerializeToArray(relivereq, sizeof(relivereq))){
					fprintf(stderr, "game serialize req failed.\n");
				}
				uint32_t relive_req_len = relivereq_.GetCachedSize();
				client->reqrelive(relivereq, relive_req_len);
				printf("relive...\n");
			}else if(action.action().compare("spellup") == 0){
				SpellLevelUpReq spellupreq_;
				spellupreq_.set_xml_id(action.xml_id());
				char spellupreq[8192];
				if(!spellupreq_.SerializeToArray(spellupreq, sizeof(spellupreq))){
					fprintf(stderr, "game serialize req failed.\n");
				}
				uint32_t spellup_req_len = spellupreq_.GetCachedSize();
				client->reqspellup(spellupreq, spellup_req_len);
				printf("spellup...\n");
			}else{
				printf("action error...\n");
			}
		}
		return bret;
	}
};



CEpollClient::CEpollClient(int iUserCount, const char* pIP, int iPort)
{
	strcpy(m_ip, pIP);
    m_iPort = iPort;
    m_iUserCount = iUserCount;
    m_iEpollFd = epoll_create(_MAX_SOCKFD_COUNT);
    m_pAllUserStatus = (struct UserStatus*)malloc(iUserCount*sizeof(struct UserStatus));
    for(int iuserid=0; iuserid<iUserCount ; iuserid++)
    {
        m_pAllUserStatus[iuserid].iUserStatus = FREE;
        sprintf(m_pAllUserStatus[iuserid].cSendbuff, "Client: %d send message \"login Game Server!\"\r\n", iuserid);
        m_pAllUserStatus[iuserid].iBuffLen = strlen(m_pAllUserStatus[iuserid].cSendbuff) + 1;
        m_pAllUserStatus[iuserid].iSockFd = -1;
    }
    memset(m_iSockFd_UserId, 0xFF, sizeof(m_iSockFd_UserId));
}

CEpollClient::~CEpollClient()
{
    free(m_pAllUserStatus);
}

int CEpollClient::ConnectToServer(int iUserId,const char *pServerIp,unsigned short uServerPort)
{
	
	client = Client::connect(pServerIp, uServerPort);
	if(client == NULL){
		printf("fail to connect to server!\n");
		google::protobuf::ShutdownProtobufLibrary(); 
		return 0;
	}
	m_pAllUserStatus[iUserId].iUserStatus = CONNECT_OK;
    m_pAllUserStatus[iUserId].iSockFd = client->getLinkFd();

    return m_pAllUserStatus[iUserId].iSockFd;
}


int CEpollClient::SendToServerData(int iUserId)
{
    //sleep(1);
	int isendsize = 1;
    if( CONNECT_OK == m_pAllUserStatus[iUserId].iUserStatus)
    {
		//login
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
				std::string str;
				pbjson::pb2json(&fres, str);
				client->currentState = str;
				
				//printf("run model =====================>");
				//char* result = NULL;
				//runModel(result,client->currentState);
				//printf("PB2Json result fres:\n%s\n\n", client->currentState.c_str());
			}
		}
        isendsize = res_len;
		if(isendsize < 0)
        {
            cout <<"[CEpollClient error]: SendToServerData, send fail, reason is:"<<strerror(errno)<<",errno is:"<<errno<<endl;
        }
        else
        {
            printf("[CEpollClient info]: iUserId: %d Send Msg Content:%s\n", iUserId, m_pAllUserStatus[iUserId].cSendbuff);
            m_pAllUserStatus[iUserId].iUserStatus = SEND_OK;
        }
    }
    return isendsize;
}

int CEpollClient::RecvFromServer(int iUserId,char *pRecvBuff,uint32_t &iBuffLen)
{
	struct  timeval start;
    struct  timeval end;
    unsigned  long diff;
    gettimeofday(&start,NULL);
	
	if(SEND_OK == m_pAllUserStatus[iUserId].iUserStatus){
		//read notice
		uint32_t notice_cmd = 0;
		log_info("debug:==============> step0");
		int notice_ret = client->readNotify(notice_cmd, pRecvBuff, iBuffLen);
		if(0 == notice_ret){
			fprintf(stderr, "[method: RecvFromServer] parse game notice succ.[notice cmd : %u]\n", notice_cmd);
		}else{
			fprintf(stderr, "[method: RecvFromServer] parse game notice fail.[notice cmd : %u]\n", notice_cmd);
		}
		log_info("debug:==============> step1");
		if(0 > iBuffLen){
			cout <<"[CEpollClient error]: iUserId: " << iUserId << "RecvFromServer, recv fail, reason is:"<<strerror(errno)<<",errno is:"<<errno<<endl;
		}else if(0 == iBuffLen){
			 cout <<"[warning:] iUserId: "<< iUserId << "RecvFromServer, STB 0" << endl;
		}else{
			//m_pAllUserStatus[iUserId].iUserStatus = RECV_OK;
		}
		log_info("debug:==============> step2");
		//char* result = NULL;
		//runModel(result,client->currentState);
		log_info("debug:==============> step3");
		log_info("PB2Json result str:%s\n", client->currentState.c_str());
	}
	gettimeofday(&end,NULL);
	diff = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
    log_info("read total cost TIME:%d ms", diff/1000);
	return iBuffLen;
}

bool CEpollClient::CloseUser(int iUserId)
{
    close(m_pAllUserStatus[iUserId].iSockFd);
    m_pAllUserStatus[iUserId].iUserStatus = FREE;
    m_pAllUserStatus[iUserId].iSockFd = -1;
    return true;
}


void CEpollClient::RunFun()
{
    int isocketfd = -1;
    for(int iuserid=0; iuserid<m_iUserCount; iuserid++)
    {
        struct epoll_event event;
        isocketfd = ConnectToServer(iuserid, m_ip, m_iPort);
        if(isocketfd < 0)
            cout <<"[CEpollClient error]: RunFun, connect fail" <<endl;
        m_iSockFd_UserId[isocketfd] = iuserid;//将用户ID和socketid关联起来

        event.data.fd = isocketfd;
        //event.events = EPOLLIN|EPOLLOUT|EPOLLERR|EPOLLHUP;
		event.events = EPOLLIN|EPOLLOUT;

        m_pAllUserStatus[iuserid].uEpollEvents = event.events;
		epoll_ctl(m_iEpollFd, EPOLL_CTL_ADD, event.data.fd, &event);
	}
	
	//pool
	workerpool = new ThreadPool(10);
    workerpool->start();
	
	while(1){
		struct epoll_event events[_MAX_SOCKFD_COUNT];
		char buffer[8096];
		memset(buffer,0,8096);
		int nfds = epoll_wait(m_iEpollFd, events, _MAX_SOCKFD_COUNT, 100 );//等待epoll事件的产生
		log_info("epoll nfds count:%d",nfds);
		for (int ifd=0; ifd<nfds; ifd++)//处理所发生的所有事件
		{
			struct epoll_event event_nfds;
			int iclientsockfd = events[ifd].data.fd;
			//cout << "events[ifd].data.fd: " << events[ifd].data.fd << endl;
			int iuserid = m_iSockFd_UserId[iclientsockfd];//根据socketfd得到用户ID
			if( events[ifd].events & EPOLLOUT )
			{
				int iret = SendToServerData(iuserid);
				cout << "iret:" << iret << endl;
				//int 
				iret = 1;
				if( 0 < iret ){
					//event_nfds.events = EPOLLIN|EPOLLERR|EPOLLHUP;
					event_nfds.events = EPOLLIN;
					event_nfds.data.fd = iclientsockfd;
					epoll_ctl(m_iEpollFd, EPOLL_CTL_MOD, event_nfds.data.fd, &event_nfds);
				}else{
					cout <<"[CEpollClient error:] EpollWait, SendToServerData fail, send iret:"<<iret<<",iuserid:"<<iuserid<<",fd:"<<events[ifd].data.fd<<endl;
					//DelEpoll(events[ifd].data.fd);
					//CloseUser(iuserid);
				}
			}else if( events[ifd].events & EPOLLIN )//监听到读事件，接收数据
			{
				uint32_t len = 8096;
				int ilen = RecvFromServer(iuserid, buffer, len);
				
				//add task
				char arg[8096];
				sprintf(arg, "%s", client->currentState.c_str());
				Mytask *taskobj=new Mytask();
				taskobj->setArg((void*)arg);
				taskobj->client = client;
				workerpool->addTask(taskobj);
				log_info("there are still %d tasks need to process\n",workerpool->size());
				
				if(0 > ilen){
					cout <<"[CEpollClient error]: RunFun, recv fail, reason is:"<<strerror(errno)<<",errno is:"<<errno<<endl;
					DelEpoll(events[ifd].data.fd);
					CloseUser(iuserid);
				}else if(0 == ilen){
					cout <<"[CEpollClient warning:] server disconnect,ilen:"<<ilen<<",iuserid:"<<iuserid<<",fd:"<<events[ifd].data.fd<<endl;
					DelEpoll(events[ifd].data.fd);
					CloseUser(iuserid);
				}else{
					m_iSockFd_UserId[iclientsockfd] = iuserid;//将socketfd和用户ID关联起来
					event_nfds.data.fd = iclientsockfd;
					//event_nfds.events = EPOLLOUT|EPOLLERR|EPOLLHUP;
					event_nfds.events = EPOLLIN;
					epoll_ctl(m_iEpollFd, EPOLL_CTL_MOD, event_nfds.data.fd, &event_nfds);
				}
				//google::protobuf::ShutdownProtobufLibrary(); 
			}else{
				cout <<"[CEpollClient error:] other epoll error"<<endl;
				//DelEpoll(events[ifd].data.fd);
				//CloseUser(iuserid);
			}
		}
		if(nfds <= 0){
			//to do default
		}
		
		//usleep(20*1000);// to avoid py run too busy
		//google::protobuf::ShutdownProtobufLibrary();
	}
}


bool CEpollClient::DelEpoll(int iSockFd)
{
	bool bret = false;
	struct epoll_event event_del;
	if(0 < iSockFd)
	{
		event_del.data.fd = iSockFd;
		event_del.events = 0;
		if( 0 == epoll_ctl(m_iEpollFd, EPOLL_CTL_DEL, event_del.data.fd, &event_del) )
		{
			bret = true;
		}else{
			cout <<"[SimulateStb error:] DelEpoll,epoll_ctl error,iSockFd:"<<iSockFd<<endl;
		}
		m_iSockFd_UserId[iSockFd] = -1;
	}else{
		bret = true;
	}
	return bret;
}
/*
bool CEpollClient::runModel(char* modreturn,std::string state)
{
	bool bret = false;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	if(tv.tv_usec % 100 >= 100){
		log_info("debug:==============> step2.1,return false,usec:%d",tv.tv_usec);
		return bret;
	}
	log_info("debug:==============> step2.1,usec:%d",tv.tv_usec);
	
	if ( !Py_IsInitialized() ){Py_Initialize();}
	if ( !Py_IsInitialized() ){return bret;}
	
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./')");
	//PyRun_SimpleString("sys.path.append('./ai_pacman/igame/')");
	PyObject *pName,*pModule,*pDict,*pFunc;
	
	//pName = PyString_FromString("modelsvrtest");
	pName = PyString_FromString("action");
	pModule = PyImport_Import(pName);
	if ( !pModule ){
		printf("can't find modelsvrtest.py");
		return bret;
	}
	
	pDict = PyModule_GetDict(pModule);
	if ( !pDict ){
		return bret;
	}
	log_info("debug:==============> step2.2");
	//pFunc = PyDict_GetItemString(pDict, "run");
	pFunc = PyDict_GetItemString(pDict, "Action");
	if ( !pFunc || !PyCallable_Check(pFunc) )
	{
		printf("can't find function [run]");
		getchar();
		return bret;
	}
	
	PyObject* pParm = PyTuple_New(1);
	PyTuple_SetItem(pParm, 0, Py_BuildValue("s",state.c_str()));
	log_info("debug:==============> step2.3");
	PyObject* ret =PyObject_CallObject(pFunc, pParm);
	log_info("debug:==============> step2.4");
	log_info("do model,state0:%s",state.c_str());
	char* result = NULL;
	if (ret != NULL && PyString_Check(ret)){
		result = PyString_AsString(ret);
	}else{
		log_info("debug:=================>PyString_Check failed...");
		return bret;
	}
	log_info("debug:==============> step2.4.1,result:%s",result);
	//char *res = NULL;
	//strncpy(res, result, 512);
	cout <<"===========================>"<<endl;
	log_info("debug:==============> step2.5");
	
	bret = doAction(result);
	
	log_info("debug:==============> step2.6");
	log_info("do model,state:%s\n actions:%s",state.c_str(),result);
	
    Py_DECREF(pFunc);
	Py_DECREF(ret);
	Py_DECREF(pParm);
	Py_DECREF(pName);
	Py_DECREF(pModule);
	log_info("debug:==============> step2.7");
    Py_Finalize();
	log_info("debug:==============> step2.8");
	return bret;
	
}
*/
/*
bool CEpollClient::doAction(char* result)
{
	bool bret = false;
	char action[8096];//todo
	sprintf(action, "{\"actionitem\":%s}", result);
	printf("action:%s\n", action);
	
	ActionList actionlist;
    std::string err;
    int ret = pbjson::json2pb(action, &actionlist, err);
	if(0 == ret){
		bret = true;
	}
	printf("actionlist size:\%d\n", actionlist.actionitem_size());
	for (int i = 0; i < actionlist.actionitem_size(); i++) {
		const Action& action = actionlist.actionitem(i);
		if(action.action().compare("move") == 0){
			printf("move...\n");
			RoomPlayerMoveReq movereq_;
			Vector *posvec = new  Vector();
			posvec->set_x(action.pos().x());
			posvec->set_z(action.pos().z());
			movereq_.set_allocated_pos(posvec);
			
			Vector *dirvec = new  Vector();
			dirvec->set_x(action.dir().x());
			dirvec->set_z(action.dir().z());
			movereq_.set_allocated_dir(dirvec);
			
			char movereq[8192];
			if(!movereq_.SerializeToArray(movereq, sizeof(movereq))){
				fprintf(stderr, "game serialize req failed.\n");
			}
			uint32_t move_req_len = movereq_.GetCachedSize();
			client->reqmove(movereq, move_req_len);
			fprintf(stderr, "game move succ.\n");
			
			RoomLoginRes gameState;
			std::string state = client->currentState;
			std::string err;
			int ret = pbjson::json2pb(state, &gameState, err);
			fprintf(stderr, "[method: %s] parse game ret:%d,error:%s.\n", "do move action",ret,err.c_str());
				for( int j = 0 ; j < gameState.snap_shot().players_size() ; j++){
				RoomPlayer* player = gameState.mutable_snap_shot()->mutable_players(j);
				if(player->id() == gameState.id()){
					Vector *posvec1 = new  Vector();
					posvec1->set_x(action.pos().x());
					posvec1->set_z(action.pos().z());
					
					Vector *dirvec1 = new  Vector();
					dirvec1->set_x(action.dir().x());
					dirvec1->set_z(action.dir().z());
					
					player->set_allocated_pos(posvec1);
					player->set_allocated_dir(dirvec1);
				}
			}
			
			//update state
			std::string new_state;
			pbjson::pb2json(&gameState, new_state);
			client->currentState = new_state;
			
			
		}else if(action.action().compare("stop") == 0){// not allow
			printf("stop...\n");
			*/
			/*
			RoomPlayerStopReq stopreq_;
			Vector *posvec1 = new  Vector();
			posvec1->set_x(action.pos().x());
			posvec1->set_z(action.pos().z());
			stopreq_.set_allocated_pos(posvec1);
			
			Vector *dirvec1 = new  Vector();
			//dirvec1->set_x(action.dir().x());
			//dirvec1->set_z(action.dir().z());
			dirvec1->set_x(0);
			dirvec1->set_z(0);
			stopreq_.set_allocated_dir(dirvec1);
			
			char stopreq[8192];
			if(!stopreq_.SerializeToArray(stopreq, sizeof(stopreq))){
				fprintf(stderr, "game serialize req failed.\n");
			}
			uint32_t stop_req_len = stopreq_.GetCachedSize();
			client->reqstop(stopreq, stop_req_len);
			*/
			/*
			fprintf(stderr, "game stop succ.\n");
		}else if(action.action().compare("spell") == 0){
			SpellStartReq spellreq_;
			spellreq_.set_xml_id(action.xml_id());
			Vector *posvec2 = new  Vector();
			posvec2->set_x(action.dir().x());
			posvec2->set_z(action.dir().z());
			spellreq_.set_allocated_pos(posvec2);
			
			Vector *dirvec2 = new  Vector();
			dirvec2->set_x(action.dir().x());
			dirvec2->set_z(action.dir().z());
			spellreq_.set_allocated_dir(dirvec2);
			
			char spellreq[8192];
			if(!spellreq_.SerializeToArray(spellreq, sizeof(spellreq))){
				fprintf(stderr, "game serialize req failed.\n");
			}
			uint32_t spell_req_len = spellreq_.GetCachedSize();
			client->reqreleasespell(spellreq, spell_req_len);
			printf("spell...\n");
		}else if(action.action().compare("relive") == 0){
			RoomPlayerReliveReq relivereq_;
			relivereq_.set_job(action.job());
			char relivereq[8192];
			if(!relivereq_.SerializeToArray(relivereq, sizeof(relivereq))){
				fprintf(stderr, "game serialize req failed.\n");
			}
			uint32_t relive_req_len = relivereq_.GetCachedSize();client->reqrelive(relivereq, relive_req_len);
			printf("relive...\n");
		}else if(action.action().compare("spellup") == 0){
			SpellLevelUpReq spellupreq_;
			spellupreq_.set_xml_id(action.xml_id());
			char spellupreq[8192];
			if(!spellupreq_.SerializeToArray(spellupreq, sizeof(spellupreq))){
				fprintf(stderr, "game serialize req failed.\n");
			}
			uint32_t spellup_req_len = spellupreq_.GetCachedSize();
			client->reqspellup(spellupreq, spellup_req_len);
			printf("spellup...\n");
		}else{
			printf("action error...\n");
		}
	}
	return bret;
}
*/