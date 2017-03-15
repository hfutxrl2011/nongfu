 
#include <pthread.h>
#include <stdarg.h>

#include "log.h"
#include "signal.h"
#include "exception.h"

void register_signal(int signo, void (*func)(int))
{
	struct sigaction sa;
	memset(&sa, 0x00, sizeof(struct sigaction));
	sa.sa_handler = func;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags |= SA_RESTART;
	if(0 != sigaction(signo, &sa, NULL))
	{
		THROW_SYS("set signal %s's resonse funciton failed [%m].", strsignal(signo));
	}
	log_frame("set signal %s's resonse funciton succ.", strsignal(signo)); 
}

void deal_with_signals(void (*sig_func)(int), unsigned int num, ...)
{
	va_list p;
	va_start(p, num);
	for(unsigned int i = 0; i < num; i++)
	{
		int signo = va_arg(p, int);
		if(SIGPIPE == signo)
		{
			register_signal(signo, SIG_IGN);
			continue;
		}
		register_signal(signo, sig_func);
	}
	va_end(p);
}

auto_mask::auto_mask(unsigned int num, ...)
{
	va_list p;
	va_start(p, num);
	_is_set = false;
	sigset_t set;
	sigemptyset(&set);
	for(unsigned int i = 0; i < num ; i++)
	{
		int signo = va_arg(p, int);
		if(0 != sigaddset(&set, signo))
		{
			log_warn("add signo to set failed [signal: %s, error: %s].", strsignal(signo), strerror(errno));
		}
	}

	if(0 != pthread_sigmask(SIG_BLOCK, &set, &_old_set))
	{
		THROW_SYS("set thread mask failed [error: %m].");
	}
	_is_set = true;
	log_frame("%s", "set thread mask succ.");
	va_end(p);
}

auto_mask::~auto_mask()
{
	do
	{
		if(_is_set)
		{
			if(0 != pthread_sigmask(SIG_SETMASK, &_old_set, NULL))
			{
				log_warn("restore thread mask failed [error: %s].", strerror(errno));
				break;
			}
			_is_set = false;
			log_frame("%s", "restore thread mask succ.");
		}
	}while(false);
}


