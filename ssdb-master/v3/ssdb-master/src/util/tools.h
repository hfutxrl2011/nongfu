/**
* @file tools.h
* @Brief common macro or function
* @author Youzu-BigApp
* @version 1.0.0
* @date 2016-03-10
*/
#ifndef UTIL_TOOLS_H_
#define UTIL_TOOLS_H_

#define SAFE_DELETE(ptr) if(NULL != (ptr)) {delete (ptr); (ptr) = NULL;}
#define SAFE_DELETE_ARR(ptr) if(NULL != (ptr)) {delete [] (ptr); (ptr) = NULL;}
#define SAFE_FREE(ptr) if(NULL != (ptr)) {free(ptr); (ptr) = NULL;}
#define UNUSED(val)  ((void)(val))

#endif
