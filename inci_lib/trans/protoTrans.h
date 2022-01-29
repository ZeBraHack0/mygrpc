#ifndef _PROTOTRANS_H_
#define _PROTOTRANS_H_

#include<string>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include "inci_lib/lib/comm.h"




namespace protoTrans {

class InciStub {
public:
    InciStub(std::string tar_str);
    ~InciStub();
    bool IncSend(google::protobuf::Message *request, google::protobuf::Message *reply);
private:
    int globalId;
    int memSz;
    int memOffset;
    int op;
    int pType;
    int dataLength;
    bool hasInit;
    comm::CommClient* client;
    void initService(int op, int ptype, int dataLength);
};
}
#endif /* _PROTOTRANS_H*/