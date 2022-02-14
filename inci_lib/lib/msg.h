#ifndef _MSG_H_
#define _MSG_H_
#include <string.h>  // strcpy
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>  // read
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define CHMOD_ID  0777
#define MSGSIZE 28

#define SERVER_TYPE 1   // 服务端发送消息类型
#define CLIENT_TYPE 2   // 客户端发送消息类型

struct Msg          // 消息结构
{
    long mtype;     // 消息类型
    char mtext[MSGSIZE]; // 消息buf
};

union semun
{
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

int createMsgQueue(int key);  // 创建消息队列
int destroyMsgQueue(int msqid); // 销毁消息队列

int getMsgQueue(int key);     // 获取消息队列

int sendMsg( int msqid, long type,  const char *_sendInfo);   // 发送消息
int recvMsg(int msqid, long type, char buf[]);       // 接收消息

int createShareMem(int key, int size); 
int destroyShareMem(int shmid);

int getShareMem(int key, int size);

int createSem(int key);
int destroySem(int semid);

int getSem(int key);

int semWait(int semid, int value);
int semPost(int semid, int value);


#endif /* _MSG_H*/