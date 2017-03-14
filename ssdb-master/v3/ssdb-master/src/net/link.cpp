/**
* @file link.cpp
* @Brief link manager
* @author Youzu-BigApp
* @version 1.0.0
* @date 2016-03-10
*/
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <netdb.h>
#include "link.h"
#include "../util/log.h"
#include "../util/tools.h"
#include "../util/exception.h"

Link::Link(){
	sock = -1;
	noblock_ = false;
	error_ = false;
	remote_ip[0] = '\0';
	remote_port = -1;
	readed = writed = 0;
}

Link::~Link(){
	this->close();
}

void Link::close(){
	if(sock >= 0){
		::close(sock);
	}
}

void Link::nodelay(bool enable){
	int opt = enable? 1 : 0;
	::setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *)&opt, sizeof(opt));
}

void Link::keepalive(bool enable){
	int opt = enable? 1 : 0;
	::setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (void *)&opt, sizeof(opt));
}

void Link::noblock(bool enable){
	noblock_ = enable;
	if(enable){
		::fcntl(sock, F_SETFL, O_NONBLOCK | O_RDWR);
	}else{
		::fcntl(sock, F_SETFL, O_RDWR);
	}
}

void Link::reset_basic_string()
{
	basic_string[0] = 0x00;
}


const char *Link::get_basic_string()
{
	return basic_string;
}

int Link::add_basic_info(const char *key, const char *val)
{
	char key_find[128];
	snprintf(key_find, sizeof(key_find), "%s:", key);
	if(NULL != strstr(basic_string, key_find)){
		return -1;
	}
	if(0x00 == basic_string[0]){
		return snprintf(basic_string, sizeof(basic_string), "%s:%s", key, val);
	}
	size_t len = strlen(basic_string);
	return snprintf(basic_string + len, sizeof(basic_string) - len, "|%s:%s", key, val);
}

int Link::add_basic_info(const char *key, uint64_t num)
{
	char str_num[128];
	snprintf(str_num, sizeof(str_num), "%lu", num);
	return add_basic_info(key, str_num);
}

static bool is_ip(const char *host){
	int dot_count = 0;
	int digit_count = 0;
	for(const char *p = host; *p; p++){
		if(*p == '.'){
			dot_count += 1;
			if(digit_count >= 1 && digit_count <= 3){ 
				digit_count = 0;
			}else{
				return false;
			}   
		}else if(*p >= '0' && *p <= '9'){
			digit_count += 1;
		}else{
			return false;
		}   
	}   
	return dot_count == 3;
}

Link* Link::connect(const char *host, int port){
	Link *link;
	int sock = -1;

	char ip_resolve[INET_ADDRSTRLEN];
	if(!is_ip(host)){
		struct hostent *hptr = gethostbyname(host);
		for(int i=0; hptr && hptr->h_addr_list[i] != NULL; i++){
			struct in_addr *addr = (struct in_addr *)hptr->h_addr_list[i];
			if(inet_ntop(AF_INET, addr, ip_resolve, sizeof(ip_resolve))){
				//printf("resolve %s: %s\n", host, ip_resolve);
				host = ip_resolve;
				break;
			}
		}
	}

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons((short)port);
	inet_pton(AF_INET, host, &addr.sin_addr);

	if((sock = ::socket(AF_INET, SOCK_STREAM, 0)) == -1){
		goto sock_err;
	}
	if(::connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1){
		goto sock_err;
	}

	////log_debug("fd: %d, connect to %s:%d", sock, ip, port);
	link = new(std::nothrow) Link();
	ASSERT_SYS(NULL != link, "try to new Link failed, no memory");
	link->sock = sock;
	link->keepalive(true);
	return link;
sock_err:
	////log_debug("connect to %s:%d failed: %s", ip, port, strerror(errno));
	if(sock >= 0){
		::close(sock);
	}
	return NULL;
}

Link* Link::listen(const char *ip, int port, int block_queue_size){
	Link *link;
	int sock = -1;

	int opt = 1;
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons((short)port);
	inet_pton(AF_INET, ip, &addr.sin_addr);

	if((sock = ::socket(AF_INET, SOCK_STREAM, 0)) == -1){
		goto sock_err;
	}
	if(::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){
		goto sock_err;
	}
	if(::bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1){
		goto sock_err;
	}
	if(::listen(sock, block_queue_size) == -1){
		goto sock_err;
	}
	////log_debug("server socket fd: %d, listen on: %s:%d", sock, ip, port);

	link = new(std::nothrow) Link();
	ASSERT_SYS(NULL != link, "try to new Link failed, no memory");
	link->sock = sock;
	snprintf(link->remote_ip, sizeof(link->remote_ip), "%s", ip);
	link->remote_port = port;
	return link;
sock_err:
	////log_debug("listen %s:%d failed: %s", ip, port, strerror(errno));
	if(sock >= 0){
		::close(sock);
	}
	return NULL;
}

Link* Link::accept(){
	Link *link;
	int client_sock;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	while((client_sock = ::accept(sock, (struct sockaddr *)&addr, &addrlen)) == -1){
		if(errno != EINTR){
			//log_error("socket %d accept failed: %s", sock, strerror(errno));
			return NULL;
		}
	}

	struct linger opt = {1, 0};
	int ret = ::setsockopt(client_sock, SOL_SOCKET, SO_LINGER, (void *)&opt, sizeof(opt));
	if (ret != 0) {
		//log_error("socket %d set linger failed: %s", client_sock, strerror(errno));
	}

	link = new(std::nothrow) Link();
	ASSERT_SYS(NULL != link, "try to new Link failed, no memory");
	if(!link->request.checkOK() || !link->request.checkOK()){
		SAFE_DELETE(link);
		log_warn("request and response buffer check failed, no memory [ fd: %d ]", client_sock);
		return NULL;
	}
	link->sock = client_sock;
	link->keepalive(true);
	inet_ntop(AF_INET, &addr.sin_addr, link->remote_ip, sizeof(link->remote_ip));
	link->remote_port = ntohs(addr.sin_port);
	readed = 0;
	writed = 0;
	link->reset_basic_string();
	return link;
}

int Link::read()
{
	int head_size = sizeof(head_t);
	int last_readed = readed;
	while(true){
		if(readed < head_size){
			int want = head_size - readed;
			int len = ::read(sock, reinterpret_cast<char *>(&request.head) + readed, want);
			if(-1 == len){
				if(errno == EINTR){
					//log_debug("%s", "try to read head from sock, -1 returned, a signal received.");
					continue;
				}else if(errno == EWOULDBLOCK || errno == EAGAIN){
					//log_debug("%s", "try to read head from sock, -1 returned, will blocked, try next time.");
					break;
				}
				//log_debug("%s", "try to read head from sock, -1 returned, error occured.");
				return -1;
			}
			if(0 == len){
				//log_debug("%s", "try to read head from sock, no data has been read, maybe peer closed, invalid format.");
				return 0;
			}
			readed += len;
		}
		if(readed >= head_size){
			int body_len = request.head.bodylen;
			if(body_len > MAX_PACKAGE_SIZE){
				log_warn("request package size is too big [ package size: %d, max: %d ].", body_len, MAX_PACKAGE_SIZE);
				return -1;   //package too big
			}
			int want = body_len + head_size - readed;
			if(want > 0){
				int len = ::read(sock, static_cast<char *>(request.body) + readed - head_size, want);
				if(-1 == len){
					if(errno == EINTR){
						//log_debug("%s", "try to read body from sock, -1 returned, a signal received.");
						continue;
					}else if(errno == EWOULDBLOCK || errno == EAGAIN){
						//log_debug("%s", "try to read body from sock, -1 returned, will blocked, try next time.");
						break;
					}
					//log_debug("%s", "try to read body from sock, -1 returned, error occured.");
					return -1;
				}
				if(0 == len){
					//log_debug("%s", "try to read body from sock, no data has been read, maybe peer closed, invalid format.");
					return 0;
				}
				readed += len;
				want -= len;
			}else{
				break;
			}
		}
		if(!noblock_){
			break;
		}
	}
	return readed - last_readed;
}
int Link::write(){
	int last_writed = writed;
	int head_size = sizeof(head_t);
	if(response.head.bodylen > MAX_PACKAGE_SIZE){
		log_warn("response package size is too big [ package size: %d, max: %d ].", response.head.bodylen, MAX_PACKAGE_SIZE);
		return -1;
	}
	while(writed < head_size + response.head.bodylen){
		if(writed < head_size){
			int want = head_size - writed;
			int len = ::write(sock, reinterpret_cast<char *>(&response.head) + writed, want);
			if(len == -1){
				if(errno == EINTR){
					continue;
				}else if(errno == EWOULDBLOCK || errno == EAGAIN){
					break;
				}
				return -1;
			}
			if(0 == len){
				//never reached here
				break;
			}
			writed += len;
		}
		if(writed >= head_size){
			int want = response.head.bodylen + head_size - writed;
			if(want > 0){
				int len = ::write(sock, static_cast<char *>(response.body) + writed - head_size, want);
				if(-1 == len){
					if(errno == EINTR){
						continue;
					}else if(errno == EWOULDBLOCK || errno == EAGAIN){
						break;
					}
					return -1;
				}
				if(0 == len){
					//never reached here
					break;
				}
				writed += len;
				want -= len;
			}else{
				break;
			}
		}
		if(!noblock_){
            break;
        }
	}
	return writed - last_writed;
}
/*
int Link::send(const head_t &head, const void *req)
{
	int want = sizeof(head_t);
	int nwrited = 0;
	while(want > 0){
		int len = ::write(sock, reinterpret_cast<const char *>(&head) + nwrited, (10 < want ? 10: want));
		if(-1 == len){
			if(EINTR == errno){
				continue;
			}
			return -1;
		}
		if(0 == len){
			return -1;
		}
		nwrited += len;
		want -= len;
	}
	printf("send head succ  body len: %d\n", head.bodylen);
	want = head.bodylen;
	nwrited = 0;
	while(want > 0){
		getchar(); int len = ::write(sock, reinterpret_cast<const char *>(req) + nwrited, (1 < want ? 1: want)); printf("send [ %d ] bytes body\n", nwrited + len);
		if(-1 == len){
			fprintf(stderr, "1error info: %s\n", strerror(errno));
			if(EINTR == errno){
				continue;
			}
			return -1;
		}
		if(0 == len){
			fprintf(stderr, "2error info: %s\n", strerror(errno));
			return -1;
		}
		nwrited += len;
		want -= len;
	}

	return head.bodylen + sizeof(head_t);
}

int Link::recv(head_t &head, void *res, int res_len)
{
	int want = sizeof(head_t);
	int nread = 0;
	while(want > 0){
		int len = ::read(sock, reinterpret_cast<char *>(&head), want);
		if(-1 == len){
			if(EINTR == errno){
				continue;
			}
			return -1;
		}
		if(0 == len){
			return -1;
		}
		nread += len;
		want -= len;
	}
	want = head.bodylen;
	nread = 0;
	if(res_len < head.bodylen){
		return -1;
	}
	printf("received head succ [ body_len: %u ].", head.bodylen);
	while(want > 0){
		int len = ::read(sock, reinterpret_cast<char *>(res), want);
		if(-1 == len){
			if(EINTR == errno){
				continue;
			}
			return -1;
		}
		if(0 == len){
			return -1;
		}
		nread += len;
		want -= len;
	}
	return head.bodylen + sizeof(head_t);
}*/


