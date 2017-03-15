 
#include "fde.h"
#include "../util/exception.h"

struct Fdevent* Fdevents::get_fde(int fd){
	while((int)events.size() <= fd){
		struct Fdevent *fde = new (std::nothrow) Fdevent();
		ASSERT_SYS(NULL != fde, "try to new Fdevent failed, no memory");
		fde->fd = events.size();
		fde->s_flags = FDEVENT_NONE;
		fde->data.num = 0;
		fde->data.ptr = NULL;
		events.push_back(fde);
	}
	return events[fd];
}


#ifdef HAVE_EPOLL
#include "fde_epoll.cpp"
#else
#include "fde_select.cpp"
#endif
