#include "inci_lib/lib/msg.h"


int commMsg(int key, int msgflag) {
    // 生成IPC 关键字
    key_t _k = key;
    int msqid = msgget(_k, msgflag); // 获取消息队列ID
    if(msqid < 0)
    {
        perror("msgget");
        return -2;
    }
    return msqid;

}


int createMsgQueue(int key)  // 创建消息队列
{
    return commMsg(key, IPC_CREAT|IPC_EXCL|CHMOD_ID);
}

int destroyMsgQueue( int msqid) // 销毁消息队列
{
    int _ret = msgctl(msqid, IPC_RMID, 0);
    if(_ret < 0)
    {
        perror("msgctl");
        return -1;
    }
    return 0;
}

int getMsgQueue(int key)     // 获取消息队列
{
    return commMsg(key, IPC_CREAT);
}

int sendMsg( int msqid, long type,  const char *_sendInfo)         // 发送消息
{
    struct Msg msg;
    msg.mtype = type;
    memcpy(msg.mtext, _sendInfo, MSGSIZE*sizeof(char));

    int _snd = msgsnd(msqid, &msg, sizeof(msg.mtext), 0);
    if( _snd < 0)
    {
        perror("msgsnd");
        return -1;
    }
    return 0;
}

int recvMsg(int msqid, long type, char buf[])          // 接收消息
{
    struct Msg msg;
    int _rcv = msgrcv(msqid, &msg, sizeof(msg.mtext), type, 0);
    if( _rcv < 0)
    {
        perror("msgrcv");
        return -1;

    }
    memcpy(buf, msg.mtext, MSGSIZE*sizeof(char));
    return 0;
}


int commShm(int key, int size, int shmflag) {
    key_t _k = key;
    int shmid = shmget(_k, size, shmflag); // 获取消息队列ID
    if(shmid < 0)
    {
        perror("shmget");
        return -2;
    }
    return shmid;

}

int createShareMem(int key, int size){
    return commShm(key, size, IPC_CREAT|IPC_EXCL|CHMOD_ID);
} 

int destroyShareMem(int shmid){
    int _ret = shmctl(shmid, IPC_RMID, 0);
    if(_ret < 0)
    {
        perror("msgctl");
        return -1;
    }
    return 0;
}

int getShareMem(int key, int size){
    return commShm(key, size, IPC_CREAT);
}



int commSem(int key, int semflag){
    // 生成IPC 关键字
    key_t _k = key;
    int semid = semget(_k, 1, semflag); // 获取消息队列ID
    if(semid < 0)
    {
        perror("semget");
        return -2;
    }
    return semid;
}

int createSem(int key){
    return commSem(key, IPC_CREAT|IPC_EXCL|CHMOD_ID);
}
int destroySem(int semid){
    int _ret = semctl(semid, 0, IPC_RMID);
    if(_ret < 0)
    {
        perror("semctl");
        return -1;
    }
    return 0;
}

int getSem(int key){
    return commSem(key, IPC_CREAT);
}

int semWait(int semid, int value){
    int nsems = 1;
    struct sembuf *semops;
    semops = (struct sembuf *)calloc(nsems, sizeof(struct sembuf));
    for (int i = 0; i < nsems; i++)
    {
        semops[i].sem_num = i;
        semops[i].sem_op = -1*value;
        semops[i].sem_flg = 0;
    }
    return semop(semid, semops, nsems);
}
int semPost(int semid, int value){
    int nsems = 1;
    struct sembuf *semops;
    semops = (struct sembuf *)calloc(nsems, sizeof(struct sembuf));
    for (int i = 0; i < nsems; i++)
    {
        semops[i].sem_num = i;
        semops[i].sem_op = value;
        semops[i].sem_flg = 0;
    }
    return semop(semid, semops, nsems);
}