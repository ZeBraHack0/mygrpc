#ifndef _COMM_H_
#define _COMM_H_
#include <signal.h>
#include <stdlib.h>

#include "msg.h"

#define SERVERID -1

namespace comm{

struct MsgArgs {
    char margs[MSGSIZE];
};

typedef void (callback)(char* data, int dataLength, char* mtext);

class CommClient{
public:
    CommClient(int key, int length); // jobid and shared memory size
    ~CommClient();  
    void PushPull(char* data, struct MsgArgs* args); // transform data and args, first two args should be key and dataLength
private:
    int dataLength;
    int key;
    int shmid;
    char *shm;
    int msgidr; // to receive message 
    int msgids; // to send message
};


class CommServer{
public:
    CommServer(callback fn);
    ~CommServer();
    void Serve();    
    void Handle(char* mtext);
private:
    int msgid; // to receive message
    callback* cb;
};

void killServer();

}
#endif /* _COMM_H*/