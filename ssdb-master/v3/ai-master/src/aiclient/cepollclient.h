#ifndef _DEFINE_EPOLLCLIENT_H_
#define _DEFINE_EPOLLCLIENT_H_


#define _MAX_SOCKFD_COUNT 65535
#include <cstring>
#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <string>
#include <string.h>
#include <stdlib.h>  
#include "stdio.h"
#include <unistd.h>

#include "AI_client.h"

#include "game.pb.h"
#include "actions.pb.h"
#include "pbjson.hpp"
#include "ThreadPool.h"
#include "../util/log.h"


using namespace std;
using namespace ai;
using namespace pb;
using namespace google::protobuf;
using namespace pbjson;

/**
 * @brief 用户状态
 */
typedef enum _EPOLL_USER_STATUS_EM
{
        FREE = 0,
        CONNECT_OK = 1,//连接成功
        SEND_OK = 2,//发送成功
        RECV_OK = 3,//接收成功
}EPOLL_USER_STATUS_EM;

/*@brief
 *@CEpollClient class 用户状态结构体
 */
struct UserStatus
{
        EPOLL_USER_STATUS_EM iUserStatus;//用户状态
        int iSockFd;//用户状态关联的socketfd
        char cSendbuff[1024];//发送的数据内容
        int iBuffLen;//发送数据内容的长度
        unsigned int uEpollEvents;//Epoll events
		std::string currentState;
};

class CEpollClient
{
        public:
                CEpollClient(int iUserCount, const char* pIP, int iPort);
                ~CEpollClient();
                void RunFun();
				Client* client;
				ThreadPool* workerpool;
        private:
                int ConnectToServer(int iUserId,const char *pServerIp,unsigned short uServerPort);
                int SendToServerData(int iUserId);
                int RecvFromServer(int iUserid,char *pRecvBuff,uint32_t &iBuffLen);
                bool CloseUser(int iUserId);
                bool DelEpoll(int iSockFd);
				//bool runModel(char* result,std::string state);
				//bool doAction(char* result);
        private:
                int    m_iUserCount;//用户数量；
                struct UserStatus *m_pAllUserStatus;//用户状态数组
                int    m_iEpollFd;//需要创建epollfd
                int    m_iSockFd_UserId[_MAX_SOCKFD_COUNT];//将用户ID和socketid关联起来
                int    m_iPort;//端口号
                char   m_ip[100];//IP地址
				
				
};


#endif
