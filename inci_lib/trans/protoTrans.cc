#include <stdlib.h>

#include "inci_lib/lib/comm.h"
#include "inci_lib/trans/protoTrans.h"
#include "inci_lib/trans/quantize.h"



namespace protoTrans {

comm::MsgArgs* createArgs(int key, int dataLength, int worker_num, int mem_sz, int mem_offset, int op){
  comm::MsgArgs *args = (comm::MsgArgs *)malloc(sizeof(comm::MsgArgs));
  memcpy(args->margs, &key, sizeof(int));
  memcpy(args->margs+sizeof(int), &dataLength, sizeof(int));
  memcpy(args->margs+2*sizeof(int), &worker_num, sizeof(int));
  memcpy(args->margs+3*sizeof(int), &mem_sz, sizeof(int));
  memcpy(args->margs+4*sizeof(int), &mem_offset, sizeof(int));
  memcpy(args->margs+5*sizeof(int), &op, sizeof(int));
  return args;
}

InciStub::InciStub(const std::string& target_str){
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
  const google::protobuf::Descriptor* requestDes = request.GetDescriptor();
  int requestFieldNum = requestDes->field_count();
  bool sendSucc = false;
  for(int i = 0; i<requestFieldNum; i++){
    const google::protobuf::FieldDescriptor* requestFdes = requestDes->field(i);
    assert(requestFdes != nullptr);  // in case of typo or something.
    if(requestFdes->type()==google::protobuf::FieldDescriptor::TYPE_MESSAGE){
      const google::protobuf::Descriptor* requestFieldDes = requestFdes->message_type();
      printf("inci field type in request: %s\n", requestFieldDes->name().c_str());
      if(requestFieldDes->name() == "aggtrArray"){
        const google::protobuf::Reflection* requestRefl = request.GetReflection();
        const google::protobuf::Message & requestField = requestRefl->GetMessage(request, requestFdes);
        const google::protobuf::FieldDescriptor* requestFieldFdes = requestFieldDes->field(0); //data field of aggtrArray
        assert(requestFieldFdes != nullptr);  // in case of typo or something.
        const google::protobuf::Reflection* requestFieldRefl = requestField.GetReflection();
        const google::protobuf::FieldDescriptor* cntForwardFdes = requestFieldDes->field(1); //data field of aggtrArray
        int cntForward = requestFieldRefl->GetInt32(requestField, cntForwardFdes);
        int sz = requestFieldRefl->FieldSize(requestField, requestFieldFdes);
        assert(sz>0);

        if(hasInit == false)initService(1,0,sz*sizeof(int),1,2000,0);
        else assert(op==1);

        char* data;
        data = (char*)malloc(sz*sizeof(int));
        auto requestArray = requestFieldRefl->GetRepeatedField<int>(requestField, requestFieldFdes);
        memcpy(data, &(requestArray[0]), sz*sizeof(int));

        comm::MsgArgs *args = createArgs(globalId, sizeof(int)*sz, cntForward, memOffset, memSz, op);
        printf("inci send data! %d %d\n", globalId, (int)(sizeof(int)*sz));
        client->PushPull(data, args);
        free(args);

        // fill reply
        const google::protobuf::Descriptor* replyDes = reply->GetDescriptor();
        int replyFieldNum = replyDes->field_count();
        for(int i = 0; i<replyFieldNum; i++){
          const google::protobuf::FieldDescriptor* replyFdes = replyDes->field(i);
          assert(replyFdes != nullptr);  // in case of typo or something.
          if(replyFdes->type()==google::protobuf::FieldDescriptor::TYPE_MESSAGE){
            const google::protobuf::Descriptor* replyFieldDes = replyFdes->message_type();
            printf("inci field type in reply: %s\n", replyFieldDes->name().c_str());
            if(replyFieldDes->name() == "aggtrArray"){
              const google::protobuf::Reflection* replyRefl = reply->GetReflection();
              google::protobuf::Message * replyField = replyRefl->MutableMessage(reply, replyFdes);
              const google::protobuf::FieldDescriptor* replyFieldFdes = replyFieldDes->field(0); //data field of aggtrArray
              assert(replyFieldFdes != nullptr);  // in case of typo or something.
              const google::protobuf::Reflection* replyFieldRefl = replyField->GetReflection();
              auto replyArray = replyFieldRefl->MutableRepeatedField<int>(replyField, replyFieldFdes);
              replyArray->Resize(sz, 0);
              memcpy(&((*replyArray)[0]), data, sz*sizeof(int));
              break;
            }
          }
        }

        free(data);
        sendSucc = true;
        break;
      } else if(requestFieldDes->name() == "aggtrMap"){
        const google::protobuf::Reflection* requestRefl = request.GetReflection();
        const google::protobuf::Message & requestField = requestRefl->GetMessage(request, requestFdes);
        const google::protobuf::FieldDescriptor* requestFieldFdes = requestFieldDes->field(0); //data field of aggtrArray
        assert(requestFieldFdes != nullptr);  // in case of typo or something.
        const google::protobuf::Reflection* requestFieldRefl = requestField.GetReflection();
        const google::protobuf::FieldDescriptor* cntForwardFdes = requestFieldDes->field(1); //data field of aggtrArray
        int cntForward = requestFieldRefl->GetInt32(requestField, cntForwardFdes);
        int sz = requestFieldRefl->FieldSize(requestField, requestFieldFdes);
        assert(sz>0);

        if(hasInit == false)initService(2,0,sz*sizeof(int),1,2000,0);
        else assert(op==2);

        char* data;
        data = (char*)malloc(sz*sizeof(int)*2);
        auto requestArray = requestFieldRefl->GetRepeatedField<int>(requestField, requestFieldFdes);
        memcpy(data, &(requestArray[0]), sz*sizeof(int));

        comm::MsgArgs *args = createArgs(globalId, sizeof(int)*sz, cntForward, 2000, 0, op);
        printf("inci send data! %d %d\n", globalId, (int)(sizeof(int)*sz));
        client->PushPull(data, args);
        free(args);

        // fill reply
        const google::protobuf::Descriptor* replyDes = reply->GetDescriptor();
        int replyFieldNum = replyDes->field_count();
        for(int i = 0; i<replyFieldNum; i++){
          const google::protobuf::FieldDescriptor* replyFdes = replyDes->field(i);
          assert(replyFdes != nullptr);  // in case of typo or something.
          if(replyFdes->type()==google::protobuf::FieldDescriptor::TYPE_MESSAGE){
            const google::protobuf::Descriptor* replyFieldDes = replyFdes->message_type();
            printf("inci field type in reply: %s\n", replyFieldDes->name().c_str());
            if(replyFieldDes->name() == "aggtrArray"){
              const google::protobuf::Reflection* replyRefl = reply->GetReflection();
              google::protobuf::Message * replyField = replyRefl->MutableMessage(reply, replyFdes);
              const google::protobuf::FieldDescriptor* replyFieldFdes = replyFieldDes->field(0); //data field of aggtrArray
              assert(replyFieldFdes != nullptr);  // in case of typo or something.
              const google::protobuf::Reflection* replyFieldRefl = replyField->GetReflection();
              auto replyArray = replyFieldRefl->MutableRepeatedField<int>(replyField, replyFieldFdes);
              replyArray->Resize(sz, 0);
              memcpy(&((*replyArray)[0]), data, sz*sizeof(int));
              break;
            } else if(replyFieldDes->name() == "aggtrMap"){
              const google::protobuf::Reflection* replyRefl = reply->GetReflection();
              google::protobuf::Message * replyField = replyRefl->MutableMessage(reply, replyFdes);
              const google::protobuf::FieldDescriptor* replyFieldFdes = replyFieldDes->field(0); //data field of aggtrArray
              assert(replyFieldFdes != nullptr);  // in case of typo or something.
              const google::protobuf::Reflection* replyFieldRefl = replyField->GetReflection();
              auto replyArray = replyFieldRefl->MutableRepeatedField<int>(replyField, replyFieldFdes);
              replyArray->Resize(sz, 0);
              memcpy(&((*replyArray)[0]), data, sz*sizeof(int));
              break;
            }
          }
        }

        free(data);
        sendSucc = true;
        break;
      }
    }
  }
  return sendSucc;
}

}