#include <stdlib.h>

#include "inci_lib/lib/comm.h"
#include "inci_lib/trans/protoTrans.h"
#include "inci_lib/trans/quantize.h"


namespace protoTrans {

comm::MsgArgs* createArgs(int key, int dataLength, int worker_num, int mem_sz, int mem_offset, int payload){
  comm::MsgArgs *args = (comm::MsgArgs *)malloc(sizeof(comm::MsgArgs));
  memcpy(args->margs, &key, sizeof(int));
  memcpy(args->margs+sizeof(int), &dataLength, sizeof(int));
  memcpy(args->margs+2*sizeof(int), &worker_num, sizeof(int));
  memcpy(args->margs+3*sizeof(int), &mem_sz, sizeof(int));
  memcpy(args->margs+4*sizeof(int), &mem_offset, sizeof(int));
  memcpy(args->margs+5*sizeof(int), &payload, sizeof(int));
  return args;
}

InciStub::InciStub(std::string target_str){
    hasInit = false;
}

InciStub::~InciStub(){
    delete(client);
}

void InciStub::initService(int new_op, int ptype, int dataLength, int globalid, int memsz, int memoffset){
  memSz = memsz;
  memOffset = memoffset;
  op = new_op;
  pType = ptype;
  globalId = globalid;
  client = new comm::CommClient(globalId, dataLength);
  hasInit = true;
}

bool InciStub::IncSend(const google::protobuf::Message &request, google::protobuf::Message *reply){
  const google::protobuf::Descriptor* gdes = request.GetDescriptor();
  int fieldNum = gdes->field_count();
  bool sendSucc = false;
  printf("inci send!\n");
  for(int i = 0; i<fieldNum; i++){
    const google::protobuf::FieldDescriptor* gfdes = gdes->field(i);
    assert(gfdes != nullptr);  // in case of typo or something.
    if(gfdes->type()==google::protobuf::FieldDescriptor::TYPE_MESSAGE){
      const google::protobuf::Descriptor* des = gfdes->message_type();
      printf("%s\n", des->name().c_str());
      if(des->name() == "aggtrArray"){
        const google::protobuf::Reflection* gref = request.GetReflection();
        const google::protobuf::Message & subRequest = gref->GetMessage(request, gfdes);
        const google::protobuf::FieldDescriptor* fdes = des->field(0);
        assert(fdes != nullptr);  // in case of typo or something.
        const google::protobuf::Reflection* ref = subRequest.GetReflection();
        int sz = ref->FieldSize(subRequest, fdes);
        assert(sz>0);
        size_t bytes = request.ByteSizeLong();

        if(hasInit == false)initService(1,0,bytes,1,2000,0);
        else assert(op==1);

        char* data;
        data = (char*)malloc(bytes);
        auto fieldRef = ref->GetRepeatedField<int>(subRequest, fdes);
        memcpy(data, &(fieldRef[0]), sz*sizeof(int));
        // switch (fdes->type())
        // {
        // case google::protobuf::FieldDescriptor::TYPE_SFIXED32:
        //   {
        //     data = (char*)malloc(sizeof(int)*sz);
        //     auto fieldRef = ref->GetRepeatedField<int>(*request, fdes);
        //     memcpy(data, &(fieldRef[0]), sz*sizeof(int));
        //     break;
        //   }
        // case google::protobuf::FieldDescriptor::TYPE_FLOAT:
        //   {
        //     data = (char*)malloc(sizeof(float)*sz);
        //     auto fieldRef = ref->GetRepeatedField<float>(*request, fdes);
        //     memcpy(data, &(fieldRef[0]), sz*sizeof(float));
        //     quan::quantizeNaive(data, sz);
        //     break;
        //   }
        // case google::protobuf::FieldDescriptor::TYPE_FIXED32:
        //   {
        //     data = (char*)malloc(sizeof(uint32_t)*sz);
        //     auto fieldRef = ref->GetRepeatedField<uint32_t>(*request, fdes);
        //     memcpy(data, &(fieldRef[0]), sz*sizeof(uint32_t));
        //     quan::signalNaive(data, sz);
        //     break;
        //   }
        // default:
        //   {
        //     printf("error type!\n");
        //     exit(-1);
        //     break;
        //   }
        // }

        comm::MsgArgs *args = createArgs(globalId, sizeof(int)*sz, 1, 2000, 0, 0);
        printf("inci send data! %d %d\n", globalId, (int)(sizeof(int)*sz));
        client->PushPull(data, args);
        free(args);

        free(data);
        sendSucc = true;
        break;
      }
    }
  }
  return sendSucc;
}

}