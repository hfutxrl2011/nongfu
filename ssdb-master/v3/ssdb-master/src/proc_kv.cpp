 
#include "serv.h"
#include "net/proc.h"
#include "net/server.h"
#include "net/link.h"
#include "util/log.h"
#include "request_example.pb.h"
#include "response_example.pb.h"

int proc_get(NetworkServer *net, Link *link, const Protoc &req, Protoc *resp){
	UNUSED(net);
	request_example_req_t freq;
	request_example_res_t fresp;
	FUNC_START
		ASSERT_PARAM(freq.ParseFromArray(req.body, req.head.bodylen), "[%s] parse request failed", link->get_basic_string());
		ASSERT_PARAM(freq.people_size() > 0, "[%s] at least, more than one person is needed.");
		log_debug("[%s] param check succ [ size: %u ].", link->get_basic_string(), freq.people_size());
		for (int i = 0; i < freq.people_size(); i++) {
			const Person& person = freq.people(i);
			char buf[1024];
			snprintf(buf, sizeof(buf), "Person ID: %d, Name: %s, Email Adress: %s", person.id(), person.name().c_str(), person.email().c_str());
			log_debug("[%s] received a request [ info: %s ].", link->get_basic_string(), buf);log_debug("[%s] received a request [ info: %s ].", link->get_basic_string(), buf);
			for (int j = 0; j < person.phones_size(); j++) {
				const Person::PhoneNumber& phone_number = person.phones(j);
				switch(static_cast<int>(phone_number.type())) {
					case Person::MOBILE:
						snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), ", Mobile Phone: %s", phone_number.number().c_str());
						break;
					case Person::HOME:
						snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), ", Home Phone: %s", phone_number.number().c_str());
						break;
					case Person::WORK:
						snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), ", Work Phone: %s", phone_number.number().c_str());
						break;
				}
			}
			log_debug("[%s] received a request [ info: %s ].", link->get_basic_string(), buf);
		}	
	FUNC_END(log_info, log_warn)
	
	int result = SSDB_E_OK;
	FUNC_GET_ERROR(result);
	fresp.set_error_code(result);
	fresp.set_error_info(get_error_info(result));
	if(SSDB_E_OK == result){
		fresp.set_return_data("I AM RETURNED DATA OF GET");
	}
	if(!fresp.SerializeToArray(resp->body, MAX_PACKAGE_SIZE)){
		log_warn("[%s] serialize result failed.", link->get_basic_string());
		resp->head.bodylen = 0;
		resp->head.status = 6; //pack returned package failed
	}else{
		log_debug("[%s] serialize result succ [ result body len: %d ].", link->get_basic_string(), fresp.GetCachedSize());
		resp->head.bodylen = fresp.GetCachedSize();
	}
	
	return SSDB_E_OK;
}

int proc_set(NetworkServer *net, Link *link, const Protoc &req, Protoc *resp){
	UNUSED(net);
	UNUSED(req);
	request_example_res_t fresp;
	fresp.set_error_code(SSDB_E_OK);
	fresp.set_error_info(get_error_info(SSDB_E_OK));
	fresp.set_return_data("I AM RETURNED DATA OF SET");
	if(!fresp.SerializeToArray(resp->body, MAX_PACKAGE_SIZE)){
		log_warn("[%s] serialize result failed.", link->get_basic_string());
		resp->head.bodylen = 0;
		resp->head.status = 6; //pack returned package failed
	}else{
		log_debug("[%s] serialize result succ [ result body len: %d ].", link->get_basic_string(), fresp.GetCachedSize());
		resp->head.bodylen = fresp.GetCachedSize();
	}
	
	return SSDB_E_OK;
}
