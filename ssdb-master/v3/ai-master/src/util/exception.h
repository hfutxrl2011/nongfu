 
#ifndef UTIL_EXCEPTION_H_
#define UTIL_EXCEPTION_H_

#include <stdlib.h>
#include <limits.h>
#include <stdarg.h>
#include <exception>
#include <string.h>
#include <stdio.h>

#define AI_E_OK 0
#define AI_E_SYS 10000
#define AI_E_PARAM 10001
#define AI_E_AUTH 10002

#define THROW_BREAK(fmt...) THROW(AI_E_OK, ##fmt)
#define THROW_SYS(fmt...) THROW(AI_E_SYS, ##fmt)
#define THROW_PARAM(fmt...) THROW(AI_E_PARAM, ##fmt)
#define THROW_AUTH(fmt...) THROW(AI_E_AUTH, ##fmt)

#define ASSERT_BREAK(expr, out_put, fmt, ...) \
	if(!expr) \
{ \
	out_put(fmt".", ##__VA_ARGS__); \
	break; \
}
#define ASSERT_LONG_BREAK(expr, fmt, ...) if(!(expr)){THROW_BREAK(fmt, ##__VA_ARGS__);}
#define ASSERT_SYS(expr, fmt, ...) if(!(expr)){THROW_SYS(fmt, ##__VA_ARGS__);}
#define ASSERT_PARAM(expr, fmt, ...) if(!(expr)){THROW_PARAM(fmt, ##__VA_ARGS__);}
#define ASSERT_AUTH(expr, fmt, ...) if(!(expr)){THROW_AUTH(fmt, ##__VA_ARGS__);}

inline const char *get_error_info(int32_t error_code)
{
	const char *ret = "internal error";
	switch (error_code){
		case AI_E_OK:
			ret = "SUCC";
			break;
		case AI_E_SYS:
			ret = "internal error";
			break;
		case AI_E_PARAM:
			ret = "invalid param";
			break;
		case AI_E_AUTH:
			ret = "auth failed";
			break;
	}
	return ret;
}

#define EXECPTION_STR_MAX_LEN 1024
class ai_exception: public std::exception
{
	public:
		ai_exception()
		{
			set_error(AI_E_OK);
			set_file("<unknown-file>");
			set_function("<unknown-function>");
			set_line(0);
			set_what("");
		}
		ai_exception(unsigned int line, const char *file, const char *function, const char *fmt, ...)
		{
			set_line(line);
			set_file(file);
			set_function(function);
			va_list p;
			va_start(p, fmt);
			set_what(fmt, p);
		}	
		ai_exception(const char *fmt, ...)
		{
			va_list p;
			va_start(p, fmt);
			set_what(fmt, p);
			va_end(p);
		}
		void set_what(const char *fmt, ...)
		{
			if(NULL == fmt || 0 == strlen(fmt))
			{
				_errmsg[0] = 0x00;
			}
			va_list p;
			va_start(p, fmt);
			vsnprintf(_errmsg, sizeof(_errmsg), fmt, p);
			va_end(p);
		}
		void set_file(const char *file)
		{
			if(NULL != file || 0 < strlen(file))
			{
				snprintf(_file, sizeof(_file), "%s", file);
			}
		}
		void set_function(const char *function)
		{		
			if(NULL != function || 0 < strlen(function))
			{
				snprintf(_function, sizeof(_function), "%s", function);
			}
		}
		void set_line(unsigned int line)
		{
			_line = line;
		}
		void set_error(int error_code)
		{
			_error_code = error_code;
		}
		virtual const char *what() const throw()
		{
			return _errmsg;
		}
		virtual const char *file() const throw()
		{
			return _file;
		}
		virtual const char *function() const throw()
		{
			return _function;
		}
		virtual unsigned int line() const throw()
		{
			return _line;
		}
		virtual int get_error()
		{
			return _error_code;
		}
	protected:
		char _file[PATH_MAX];
		char _function[PATH_MAX];
		unsigned int _line;
		int _error_code;
		char _errmsg[EXECPTION_STR_MAX_LEN];
};
#define PREPARE_ERROR_STRING(error_code, right_output, error_output) \
	char buf[EXECPTION_STR_MAX_LEN]; \
snprintf(buf, sizeof(buf), "%s", 0 == strcmp(ex.what(), "") ? "an unexpect error occured" : ex.what()); \
snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "%s", " BREAK_POINT_INFO["); \
if(0 != strcmp("<unknown-file>", ex.file())) \
{ \
	snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "file: %s, ", ex.file()); \
} \
if(0 < ex.line()) \
{ \
	snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "line: %d, ", ex.line()); \
} \
if(0 != strcmp("<unknown-function>", ex.function())) \
{ \
	snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "function: %s, ", ex.function()); \
} \
snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "error_code: %d]", (error_code)); \
if(AI_E_OK == error_code) \
{ \
	right_output("%s.", buf); \
} \
else \
{ \
	error_output("%s.", buf); \
} \
__func_ret_code_by_wu_jing_baidu_inf_iis_not_same_as_other_variable__ = (error_code); \

#define FUNC_START \
	int __func_ret_code_by_wu_jing_baidu_inf_iis_not_same_as_other_variable__ = AI_E_SYS; \
try \
{ \
	do \
	{
#define FUNC_END(right_output, error_output) \
	}while(false); \
	__func_ret_code_by_wu_jing_baidu_inf_iis_not_same_as_other_variable__ = AI_E_OK; \
	right_output("main logic was executed successfully [file: %s, function: %s].", __FILE__, __FUNCTION__); \
} \
catch(ai_exception &ex) \
{ \
	PREPARE_ERROR_STRING(ex.get_error(), right_output, error_output); \
} \
catch(...) \
{ \
	error_output("an error occured [error_info: unknown, error_code: %d].", \
			__func_ret_code_by_wu_jing_baidu_inf_iis_not_same_as_other_variable__); \
}

#define THROW(error_code, fmt, ...) \
	do \
{ \
	ai_exception tmp; \
	tmp.set_file(__FILE__); \
	tmp.set_line(__LINE__); \
	tmp.set_error(error_code); \
	tmp.set_function(__FUNCTION__); \
	tmp.set_what(fmt, ##__VA_ARGS__); \
	throw tmp; \
}while(false)     

#define FUNC_GET_ERROR(result) \
	result = __func_ret_code_by_wu_jing_baidu_inf_iis_not_same_as_other_variable__;

#define FUNC_RETURN \
	return __func_ret_code_by_wu_jing_baidu_inf_iis_not_same_as_other_variable__;	

#endif


