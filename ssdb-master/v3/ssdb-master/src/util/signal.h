/**
* @file signal.h
* @Brief signal hander
* @author Youzu-BigApp
* @version 1.0.0
* @date 2016-03-10
*/
#ifndef UTIL_SIGNAL_H_
#define UTIL_SIGNAL_H_

#include <signal.h>

void sig_func(int signo);
void register_signal(int signo, void (*func)(int));
void deal_with_signals(void (*sig_func)(int), unsigned int num, ...);
class auto_mask
{
	public:
		auto_mask(unsigned int num, ...);
		~auto_mask();
	private:
		bool _is_set;
		sigset_t _old_set;
};

#endif
