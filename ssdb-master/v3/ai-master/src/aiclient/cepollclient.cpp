#include <Python.h>
#include "cepollclient.h"
#include <cstring>
#include <iostream>

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
				//m_pAllUserStatus[iUserId].gameState = &fres;
				//std::string str;
				//pbjson::pb2json(m_pAllUserStatus[iUserId].gameState, str);
				//update game state
				client->setGameState(&fres);
				//printf("run model =====================>");
				char* result = NULL;
				//runModel(result,&fres);
				//printf("PB2Json result fres:\n%s\n\n", str.c_str());
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
	if(SEND_OK == m_pAllUserStatus[iUserId].iUserStatus){
		//read notice
		uint32_t notice_cmd = 0;
		int notice_ret = client->readNotify(notice_cmd, pRecvBuff, iBuffLen);
		if(0 == notice_ret){
			fprintf(stderr, "[method: RecvFromServer] parse game notice succ.[notice cmd : %u]\n", notice_cmd);
		}else{
			fprintf(stderr, "[method: RecvFromServer] parse game notice fail.[notice cmd : %u]\n", notice_cmd);
		}
		if(0 > iBuffLen){
			cout <<"[CEpollClient error]: iUserId: " << iUserId << "RecvFromServer, recv fail, reason is:"<<strerror(errno)<<",errno is:"<<errno<<endl;
		}else if(0 == iBuffLen){
			 cout <<"[warning:] iUserId: "<< iUserId << "RecvFromServer, STB 0" << endl;
		}else{
			//m_pAllUserStatus[iUserId].iUserStatus = RECV_OK;
		}
		std::string str;
		RoomLoginRes* state = client->getGameState();
		printf("run model =====================>size:%d",state->snap_shot().players_size());
		{
			for (int i = 0; i < state->snap_shot().players_size(); i++) {
				const RoomPlayer& player = state->snap_shot().players(i);
				
				std::string str1;
				pbjson::pb2json(&player, str1);
				//int32_t uid = player.id();
				fprintf(stderr, "[method: %s] i:%d,str:%s","p2====",i,str1.c_str());
				/*
				const char *nickname = player.nickname().c_str();
				Vector pos = player.pos();
				
				for (int i = 0; i < player.spells_size(); i++) {
					const RoomPlayerSpell& spell = player.spells(i);
					fprintf(stderr, "[method: %s] i:%d,xml_id:%d,cd:%d,level:%d","p22",i,spell.xml_id(),spell.cd(),spell.level());
				}
				fprintf(stderr, "[method: %s] parse game result succ [ error_code: %d, "
					"nickname: %s ,x:%d,z:%d].\n", "get1", uid, nickname,player.pos().x(),player.pos().z());
				*/
			}
		}
		//pbjson::pb2json(client->getGameState(), str);
		//std::string str1;
		//pbjson::pb2json(m_pAllUserStatus[iUserId].gameState, str1);
		
		
		char* result = NULL;
		//runModel(result,state);
		//printf("PB2Json result str:\n%s,%d ,result:\n", str.c_str(),state->id());
	}
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
        event.events = EPOLLIN|EPOLLOUT|EPOLLERR|EPOLLHUP;

        m_pAllUserStatus[iuserid].uEpollEvents = event.events;
		epoll_ctl(m_iEpollFd, EPOLL_CTL_ADD, event.data.fd, &event);
	}
	while(1){
		struct epoll_event events[_MAX_SOCKFD_COUNT];
		char buffer[8096];
		memset(buffer,0,8096);
		int nfds = epoll_wait(m_iEpollFd, events, _MAX_SOCKFD_COUNT, 100 );//等待epoll事件的产生
		for (int ifd=0; ifd<nfds; ifd++)//处理所发生的所有事件
		{
			struct epoll_event event_nfds;
			int iclientsockfd = events[ifd].data.fd;
			cout << "events[ifd].data.fd: " << events[ifd].data.fd << endl;
			int iuserid = m_iSockFd_UserId[iclientsockfd];//根据socketfd得到用户ID
			if( events[ifd].events & EPOLLOUT )
			{
				int iret = SendToServerData(iuserid);
				cout << "iret:" << iret << endl;
				//int 
				iret = 1;
				if( 0 < iret ){
					event_nfds.events = EPOLLIN|EPOLLERR|EPOLLHUP;
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
				cout<<ilen<<endl;
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
					event_nfds.events = EPOLLOUT|EPOLLERR|EPOLLHUP;
					epoll_ctl(m_iEpollFd, EPOLL_CTL_MOD, event_nfds.data.fd, &event_nfds);
				}
			}else{
				cout <<"[CEpollClient error:] other epoll error"<<endl;
				DelEpoll(events[ifd].data.fd);
				CloseUser(iuserid);
			}
		}
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

bool CEpollClient::runModel(char* result,RoomLoginRes* gameState)//RoomLoginRes *gameState
{
	bool bret = false;
	std::string jsonstr;
	pbjson::pb2json(gameState, jsonstr);
	
	Py_Initialize();
	if ( !Py_IsInitialized() ){return bret;}
	
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./')");
	PyObject *pName,*pModule,*pDict,*pFunc;
	
	pName = PyString_FromString("modelsvrtest");
	pModule = PyImport_Import(pName);
	if ( !pModule ){
		printf("can't find modelsvrtest.py");
		return bret;
	}
	
	pDict = PyModule_GetDict(pModule);
	if ( !pDict ){
		return bret;
	}
	pFunc = PyDict_GetItemString(pDict, "run");
	if ( !pFunc || !PyCallable_Check(pFunc) )
	{
		printf("can't find function [run]");
		getchar();
		return bret;
	}
	
	PyObject* pParm = PyTuple_New(1);
	PyTuple_SetItem(pParm, 0, Py_BuildValue("s",jsonstr.c_str()));
	
	PyObject* ret =PyObject_CallObject(pFunc, pParm);
	
	result = PyString_AsString(ret);
    printf("result:%s\n", result);
    Py_DECREF(pFunc);
    Py_Finalize();
	
	cout <<"===========================>"<<endl;
	bret = doAction(result);
	return bret;
	
}

bool CEpollClient::doAction(char* result)
{
	bool bret = false;
	char action[100];//todo
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
		}else if(action.action().compare("stop") == 0){
			printf("stop...\n");
			RoomPlayerStopReq stopreq_;
			Vector *posvec1 = new  Vector();
			posvec1->set_x(action.pos().x());
			posvec1->set_z(action.pos().z());
			stopreq_.set_allocated_pos(posvec1);
			
			Vector *dirvec1 = new  Vector();
			dirvec1->set_x(action.dir().x());
			dirvec1->set_z(action.dir().z());
			stopreq_.set_allocated_dir(dirvec1);
			
			char stopreq[8192];
			if(!stopreq_.SerializeToArray(stopreq, sizeof(stopreq))){
				fprintf(stderr, "game serialize req failed.\n");
			}
			uint32_t stop_req_len = stopreq_.GetCachedSize();
			client->reqstop(stopreq, stop_req_len);
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
