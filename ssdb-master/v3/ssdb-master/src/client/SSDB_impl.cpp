#include "SSDB_impl.h"
#include "util/strings.h"
#include <signal.h>

namespace ssdb{

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

void* ClientImpl::request(const std::string &cmd, const void *req, uint32_t len, void *res, uint32_t &res_len){
	uint32_t res_buf_len = res_len;
	res_len = 0;
	head_t req_head;
	req_head.logid = 123456;
	req_head.magic_num = 13579;
	snprintf(req_head.method, sizeof(req_head.method), "%s", cmd.c_str());
	req_head.status = 0;
	req_head.version = 1;
	req_head.bodylen = len;
	link->readed = link->writed = 0;
	int fd = link->fd();
	int left = sizeof(req_head);
	int total = left;
	while(left > 0){
		int wret = ::write(fd, reinterpret_cast<const char *>(&req_head) + total - left, left);
		if(-1 == wret){
			if(EINTR == errno){
				fprintf(stderr, "write failed [ fd: %d, error: %s ].\n", fd, strerror(errno));
				continue;
			}
			return NULL;
		}
		left -= wret;
	}
	left = len;
	total = left;
	while(left > 0){
		int wret = ::write(fd, static_cast<const char *>(req) + total - left, left);
		if(-1 == wret){
			if(EINTR == errno){
				continue;
			}
			fprintf(stderr, "write failed [ fd: %d, error: %s ].\n", fd, strerror(errno));
			return NULL;
		}
		left -= wret;
	}
	

	head_t res_head;
	left = sizeof(head_t);
	total = left;
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
	if(res_head.status != 0){
		fprintf(stderr, "request returned with error status [ status: %u ].\n", res_head.status);
		return NULL;
	}
	left = res_head.bodylen;
	total = left;
	if(res_buf_len < total){
		fprintf(stderr, "response data is too big [ res_buf_len: %u, total: %u ].\n", res_buf_len, total);
		return NULL;
	}
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
	res_len = res_head.bodylen;
	fprintf(stderr, "request finished [ method: %s, res_len: %u ].\n", cmd.c_str(), res_len);	
	return res;
}

int ClientImpl::get(const void *req, uint32_t req_len, void *res, uint32_t &res_len)
{
	std::string cmd = "get";
	if(NULL == request(cmd, req, req_len, res, res_len)){
		return -1;
	}	
	return 0;
}

int ClientImpl::set(const void *req, uint32_t req_len, void *res, uint32_t &res_len)
{
	std::string cmd = "set";
	if(NULL == request(cmd, req, req_len, res, res_len)){
		return -1;
	}	
	return 0;
}

}; // namespace ssdb
