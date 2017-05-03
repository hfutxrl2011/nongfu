 
#ifndef NET_PROTOC_H_
#define NET_PROTOC_H_

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_PACKAGE_SIZE 8192

typedef struct _head_t
{
	char method[32];
    char provider[32];
    uint8_t version;
    uint8_t status; //0: succ, 1: method does not exists, 2: magic number error, 3: request package too big, 4: response package too big, 5: pack returned package failed, 6: other system error
    uint32_t magic_num;
    uint32_t reserved;
    uint32_t bodylen;
    uint64_t logid;
	_head_t()
	{
		memset(this, 0x00, sizeof(struct _head_t));
	}
}head_t;

typedef struct _head_game
{
	
    uint32_t Flags;
	uint32_t Length;
	uint32_t Command;
	_head_game()
	{
		memset(this, 0x00, sizeof(struct _head_game));
	}
}head_game;

typedef struct _ProtocGame
{
	head_game head;
	void *body;
	_ProtocGame()
	{
		body = malloc(MAX_PACKAGE_SIZE);
	}
	~_ProtocGame()
	{
		if(NULL != body){
			free(body);
			body = NULL;
		}
		memset(&head, 0x00, sizeof(head_game));
	}
	void reset()
	{
		memset(&head, 0x00, sizeof(head_game));
	}
	bool checkOK()
	{
		if(NULL == body){
			return false;
		}
		return true;
	}
}ProtocGame;

typedef struct _Protoc
{
	head_t head;
	void *body;
	_Protoc()
	{
		body = malloc(MAX_PACKAGE_SIZE);
	}
	~_Protoc()
	{
		if(NULL != body){
			free(body);
			body = NULL;
		}
		memset(&head, 0x00, sizeof(head_t));
	}
	void reset()
	{
		memset(&head, 0x00, sizeof(head_t));
	}
	bool checkOK()
	{
		if(NULL == body){
			return false;
		}
		return true;
	}
}Protoc;
#endif
