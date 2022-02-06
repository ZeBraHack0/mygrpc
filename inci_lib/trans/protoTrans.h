#ifndef _PROTOTRANS_H_
#define _PROTOTRANS_H_

#include<string>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include "inci_lib/lib/comm.h"




namespace protoTrans {

class InciStub {
public:
    InciStub(const std::string& tar_str);
    ~InciStub();
    bool IncSend(const google::protobuf::Message &request, google::protobuf::Message *reply);
private:
    int globalId;
    int memSz;
    int memOffset;
    int op;
    int pType;
    int dataLength;
    bool hasInit;
    comm::CommClient* client;
    void initService(int new_op, int ptype, int dataLength, int globalid, int memsz, int memoffset);
};
}
#endif /* _PROTOTRANS_H*/