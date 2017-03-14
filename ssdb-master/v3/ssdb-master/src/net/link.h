/**
* @file link.h
* @Brief link manager
* @author Youzu-BigApp
* @version 1.0.0
* @date 2016-03-10
*/
#ifndef NET_LINK_H_
#define NET_LINK_H_

#include <vector>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "proc.h"
#include "protoc.h"

class Link{
	private:
		int sock;
		bool noblock_;
		bool error_;
		char basic_string[1024];
		
		static int min_recv_buf;
		static int min_send_buf;
	public:
		char remote_ip[INET_ADDRSTRLEN];
		int remote_port;

		Protoc request;
		Protoc response;
		int readed;
		int writed;

		double create_time;
		double active_time;

		Link();
		~Link();
		void close();
		void nodelay(bool enable=true);
		void noblock(bool enable=true);
		void keepalive(bool enable=true);
		const char *get_basic_string();
		int add_basic_info(const char *key, const char *val);
		int add_basic_info(const char *key, uint64_t num);
		void reset_basic_string();

		int fd() const{
			return sock;
		}
		bool error() const{
			return error_;
		}
		void mark_error(){
			error_ = true;
		}

		static Link* connect(const char *ip, int port);
		static Link* listen(const char *ip, int port, int block_queue_size);
		Link* accept();

		// read network data info buffer
		int read();
		int write();
		//int send(const head_t &head, const void *req);
		//int recv(head_t &head, void *res, int res_len);
};

#endif
