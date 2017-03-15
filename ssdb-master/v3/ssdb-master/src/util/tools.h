 
#ifndef UTIL_TOOLS_H_
#define UTIL_TOOLS_H_

#define SAFE_DELETE(ptr) if(NULL != (ptr)) {delete (ptr); (ptr) = NULL;}
#define SAFE_DELETE_ARR(ptr) if(NULL != (ptr)) {delete [] (ptr); (ptr) = NULL;}
#define SAFE_FREE(ptr) if(NULL != (ptr)) {free(ptr); (ptr) = NULL;}
#define UNUSED(val)  ((void)(val))

#endif
