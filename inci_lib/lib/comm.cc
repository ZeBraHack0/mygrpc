#include <assert.h>
#include <thread>

#include "inci_lib/lib/comm.h"

namespace comm{
CommClient::CommClient(int k, int length){
    key = k;
    dataLength = length;
    if(length > 0){
        shmid = createShareMem(key, dataLength);
        shm = (char*)shmat(shmid, NULL, 0);
    }
    msgidr = createMsgQueue(key); // to receive message 
    msgids = getMsgQueue(SERVERID); // to send message
    printf("[comm]proc communication client starts!\n");
}

CommClient::~CommClient(){
    if(dataLength > 0){
        if(shmdt(shm) == -1)
        {
            fprintf(stderr, "shmdt failed\n");
            exit(EXIT_FAILURE);
        }
        destroyShareMem(shmid);
    }
    destroyMsgQueue(msgidr);
}

void CommClient::PushPull(char* data, struct MsgArgs* args){
    char *mtext = (char*)malloc(MSGSIZE * sizeof(char));
    char *mresponse = (char*)malloc(MSGSIZE * sizeof(char));
    int curKey = 0, curLen = 0;

    memcpy(mtext, args->margs, sizeof(MsgArgs));
    memcpy(&curKey, args->margs, sizeof(int));
    memcpy(&curLen, args->margs+sizeof(int), sizeof(int));
    assert(curKey==key);
    assert(dataLength>=curLen);
    printf("[comm]copy parameters %d %d success!\n", curKey, curLen);
    if(curLen)memcpy(shm, data, curLen);
    //printf("[comm]copy data success!\n");
    sendMsg(msgids, 1, mtext);
    recvMsg(msgidr, 1, mresponse);
    memcpy(data, shm, curLen);
    printf("[comm]push and pull data success!\n");
    free(mtext);
    free(mresponse);
}


int kill = 0;

void killServer(){
    kill = 1;
    int key = -2, dataLength = 0;
    comm::MsgArgs *args = (comm::MsgArgs *)malloc(sizeof(comm::MsgArgs));
    memcpy(args->margs, &key, sizeof(int));
    memcpy(args->margs+sizeof(int), &dataLength, sizeof(int));

    comm::CommClient client(key, dataLength);
    client.PushPull(NULL, args);
    free(args);
}

static void
signal_handler(int signum) {
    if (signum == SIGINT || signum == SIGTERM) {
        printf("\n");
        printf("Signal %d received, preparing to exit...\n", signum);
    }
    killServer();
}

CommServer::CommServer(callback* fn){
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    msgid = createMsgQueue(SERVERID);
    cb = fn;
}

CommServer::~CommServer(){
    destroyMsgQueue(msgid);
}

void CommServer::Handle(char* mtext){
    int dataLength = 0, key = 0;
    memcpy(&key, mtext, sizeof(int));
    memcpy(&dataLength, mtext+sizeof(int), sizeof(int));
    printf("[comm]server parameters: %d, %d\n", key, dataLength);
    if(dataLength == 0){
        printf("[comm]empty message! Close the Server\n");
        int msgids = getMsgQueue(key);
        sendMsg(msgids, 1, mtext);
        free(mtext);
    } else {
        int shmid = getShareMem(key, dataLength);
        char * shm = (char*)shmat(shmid, NULL, 0);
        cb(shm, dataLength, mtext);
        int msgids = getMsgQueue(key);
        sendMsg(msgids, 1, mtext);
        free(mtext);
        if(shmdt(shm) == -1)
        {
            fprintf(stderr, "shmdt failed\n");
            exit(EXIT_FAILURE);
        }
    }
}

void HandleHelper(CommServer* cs, char* mtext){
    printf("[comm]receive requests!\n");
    cs->Handle(mtext);
}

void CommServer::Serve(){
    printf("[comm]proc communication server starts!\n");
    while(!kill){
        char *mtext = (char*)malloc(MSGSIZE* sizeof(char));
        recvMsg(msgid, 0, mtext);
        std::thread* t = new std::thread(&HandleHelper, this, mtext);
        t->detach();
    }
    printf("[comm]proc communication server ends!\n");
}

}