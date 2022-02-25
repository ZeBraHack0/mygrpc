#include <stdlib.h>

#include <google/protobuf/map_field.h>

#include "inci_lib/lib/comm.h"
#include "inci_lib/trans/protoTrans.h"
#include "inci_lib/trans/quantize.h"



namespace protoTrans {

comm::MsgArgs* createArgs(int key, int dataLength, int worker_num, int mem_sz, int mem_offset, int op, int ptype, int flag){
  comm::MsgArgs *args = (comm::MsgArgs *)malloc(sizeof(comm::MsgArgs));
  memcpy(args->margs, &key, sizeof(int));
  memcpy(args->margs+sizeof(int), &dataLength, sizeof(int));
  memcpy(args->margs+2*sizeof(int), &worker_num, sizeof(int));
  memcpy(args->margs+3*sizeof(int), &mem_sz, sizeof(int));
  memcpy(args->margs+4*sizeof(int), &mem_offset, sizeof(int));
  memcpy(args->margs+5*sizeof(int), &op, sizeof(int));
  memcpy(args->margs+6*sizeof(int), &ptype, sizeof(int));
  memcpy(args->margs+7*sizeof(int), &flag, sizeof(int));
  return args;
}

InciStub::InciStub(const std::string& target_str){
    hasInit = false;
}

InciStub::~InciStub(){
    if(hasInit)delete(client);
}

void InciStub::initService(int new_op, int ptype, int datalength, int globalid, int memsz, int memoffset){
  memSz = memsz;
  memOffset = memoffset;
  op = new_op;
  dataLength = datalength;
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
      if(requestFieldDes->name() == "AggtrArray"){
        const google::protobuf::Reflection* requestRefl = request.GetReflection();
        const google::protobuf::Message & requestField = requestRefl->GetMessage(request, requestFdes);
        const google::protobuf::FieldDescriptor* requestFieldFdes = requestFieldDes->field(0); //data field of AggtrArray
        assert(requestFieldFdes != nullptr);  // in case of typo or something.
        const google::protobuf::Reflection* requestFieldRefl = requestField.GetReflection();

        const google::protobuf::FieldDescriptor* confInfoFdes = requestFieldDes->field(1); //conf field of AggtrArray
        const google::protobuf::Descriptor* confInfoDes = confInfoFdes->message_type();
        const google::protobuf::Message & confInfo = requestFieldRefl->GetMessage(requestField, confInfoFdes);
        const google::protobuf::Reflection* confInfoRefl = confInfo.GetReflection();
        const google::protobuf::FieldDescriptor* cntForwardFdes = confInfoDes->field(0);
        int cntForward = confInfoRefl->GetInt32(confInfo, cntForwardFdes);
        const google::protobuf::FieldDescriptor* pTypeFdes = confInfoDes->field(1); //data field of AggtrArray
        int pType = confInfoRefl->GetInt32(confInfo, pTypeFdes);

        int sz = requestFieldRefl->FieldSize(requestField, requestFieldFdes);
        assert(sz>0);

        if(hasInit == false)initService(1,pType,sz*sizeof(int),1,2000,0);
        else assert(op==1);

        char* data;
        data = (char*)malloc(sz*sizeof(int));
        auto requestArray = requestFieldRefl->GetRepeatedField<int>(requestField, requestFieldFdes);
        memcpy(data, &(requestArray[0]), sz*sizeof(int));

        comm::MsgArgs *args = createArgs(globalId, sizeof(int)*sz, cntForward, memSz, memOffset, op, pType, 0);
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
            if(replyFieldDes->name() == "AggtrArray"){
              const google::protobuf::Reflection* replyRefl = reply->GetReflection();
              google::protobuf::Message * replyField = replyRefl->MutableMessage(reply, replyFdes);
              const google::protobuf::FieldDescriptor* replyFieldFdes = replyFieldDes->field(0); //data field of AggtrArray
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
      } else if(requestFieldDes->name() == "AggtrMap"){
        const google::protobuf::Reflection* requestRefl = request.GetReflection();
        const google::protobuf::Message & requestField = requestRefl->GetMessage(request, requestFdes);
        const google::protobuf::FieldDescriptor* requestFieldFdes = requestFieldDes->field(0); //data field of AggtrArray
        assert(requestFieldFdes != nullptr);  // in case of typo or something.
        const google::protobuf::Reflection* requestFieldRefl = requestField.GetReflection();
        const google::protobuf::FieldDescriptor* confInfoFdes = requestFieldDes->field(1); //conf field of AggtrArray
        const google::protobuf::Descriptor* confInfoDes = confInfoFdes->message_type();
        const google::protobuf::Message & confInfo = requestFieldRefl->GetMessage(requestField, confInfoFdes);
        const google::protobuf::Reflection* confInfoRefl = confInfo.GetReflection();
        const google::protobuf::FieldDescriptor* cntForwardFdes = confInfoDes->field(0);
        int cntForward = confInfoRefl->GetInt32(confInfo, cntForwardFdes);
        const google::protobuf::FieldDescriptor* pTypeFdes = confInfoDes->field(1); //data field of AggtrArray
        int pType = confInfoRefl->GetInt32(confInfo, pTypeFdes);
        int sz = requestFieldRefl->FieldSize(requestField, requestFieldFdes);
        assert(sz>0);

        if(hasInit == false)initService(2,pType,sz*sizeof(int),1,2000,0);
        else assert(op==2);

        char* data;
        data = (char*)malloc(sz*sizeof(int));
        auto requestArray = requestFieldRefl->GetRepeatedField<int>(requestField, requestFieldFdes);
        memcpy(data, &(requestArray[0]), sz*sizeof(int));

        comm::MsgArgs *args = createArgs(globalId, sizeof(int)*sz, cntForward, memSz, memOffset, op, pType, 0);
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
            if(replyFieldDes->name() == "AggtrMap"){
              const google::protobuf::Reflection* replyRefl = reply->GetReflection();
              google::protobuf::Message * replyField = replyRefl->MutableMessage(reply, replyFdes);
              const google::protobuf::FieldDescriptor* replyFieldFdes = replyFieldDes->field(0); //data field of AggtrArray
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
      } else if(requestFieldDes->name() == "MonitorMap"){
        const google::protobuf::Reflection* requestRefl = request.GetReflection();
        google::protobuf::Message * requestField = requestRefl->MutableMessage(const_cast<google::protobuf::Message*>(&request), requestFdes);
        const google::protobuf::FieldDescriptor* requestFieldFdes = requestFieldDes->field(0); //data field of AggtrArray
        assert(requestFieldFdes != nullptr);  // in case of typo or something.
        const google::protobuf::Reflection* requestFieldRefl = requestField->GetReflection();
        const google::protobuf::FieldDescriptor* confInfoFdes = requestFieldDes->field(1); //conf field of AggtrArray
        const google::protobuf::Descriptor* confInfoDes = confInfoFdes->message_type();
        const google::protobuf::Message & confInfo = requestFieldRefl->GetMessage(*requestField, confInfoFdes);
        const google::protobuf::Reflection* confInfoRefl = confInfo.GetReflection();
        const google::protobuf::FieldDescriptor* cntForwardFdes = confInfoDes->field(0);
        int cntForward = confInfoRefl->GetInt32(confInfo, cntForwardFdes);
        const google::protobuf::FieldDescriptor* pTypeFdes = confInfoDes->field(1); //data field of AggtrArray
        int pType = confInfoRefl->GetInt32(confInfo, pTypeFdes);
        int sz = requestFieldRefl->FieldSize(*requestField, requestFieldFdes);
        assert(sz>0);

        if(hasInit == false)initService(3,pType,2*sz*sizeof(int),1,2000,0);
        else assert(op==3);

        char* data;
        data = (char*)malloc(sz*sizeof(int)*2);
        auto it = requestFieldRefl->MapBegin(requestField, requestFieldFdes);
        for(int i = 0; i<sz; i++){
          int tmp_key = it.GetKey().GetInt32Value();
          memcpy(data+2*i*sizeof(int), &tmp_key, sizeof(int));
          int tmp_value = it.GetValueRef().GetInt32Value();
          memcpy(data+(2*i+1)*sizeof(int), &tmp_value, sizeof(int));
          it++;
        }

        comm::MsgArgs *args = createArgs(globalId, 2*sizeof(int)*sz, cntForward, memSz, memOffset, op, pType, 0);
        printf("inci send data! %d %d\n", globalId, 2*(int)(sizeof(int)*sz));
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
            if(replyFieldDes->name() == "MonitorMap"){
              const google::protobuf::Reflection* replyRefl = reply->GetReflection();
              google::protobuf::Message * replyField = replyRefl->MutableMessage(reply, replyFdes);
              const google::protobuf::FieldDescriptor* replyFieldFdes = replyFieldDes->field(0); //data field of AggtrArray
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
      } else if(requestFieldDes->name() == "VoteMap"){
        const google::protobuf::Reflection* requestRefl = request.GetReflection();
        google::protobuf::Message * requestField = requestRefl->MutableMessage(const_cast<google::protobuf::Message*>(&request), requestFdes);
        const google::protobuf::FieldDescriptor* requestFieldFdes = requestFieldDes->field(0); //data field of AggtrArray
        assert(requestFieldFdes != nullptr);  // in case of typo or something.
        const google::protobuf::Reflection* requestFieldRefl = requestField->GetReflection();
        const google::protobuf::FieldDescriptor* confInfoFdes = requestFieldDes->field(1); //conf field of AggtrArray
        const google::protobuf::Descriptor* confInfoDes = confInfoFdes->message_type();
        const google::protobuf::Message & confInfo = requestFieldRefl->GetMessage(*requestField, confInfoFdes);
        const google::protobuf::Reflection* confInfoRefl = confInfo.GetReflection();
        const google::protobuf::FieldDescriptor* cntForwardFdes = confInfoDes->field(0);
        int cntForward = confInfoRefl->GetInt32(confInfo, cntForwardFdes);
        const google::protobuf::FieldDescriptor* pTypeFdes = confInfoDes->field(1); //data field of AggtrArray
        int pType = confInfoRefl->GetInt32(confInfo, pTypeFdes);
        int sz = requestFieldRefl->FieldSize(*requestField, requestFieldFdes);
        assert(sz>0);

        if(hasInit == false)initService(4,pType,2*sz*sizeof(int),1,2000,0);
        else assert(op==4);

        char* data;
        data = (char*)malloc(sz*sizeof(int)*2);
        auto it = requestFieldRefl->MapBegin(requestField, requestFieldFdes);
        for(int i = 0; i<sz; i++){
          int tmp_key = it.GetKey().GetInt32Value();
          memcpy(data+2*i*sizeof(int), &tmp_key, sizeof(int));
          int tmp_value = it.GetValueRef().GetInt32Value();
          memcpy(data+(2*i+1)*sizeof(int), &tmp_value, sizeof(int));
          it++;
        }

        comm::MsgArgs *args = createArgs(globalId, 2*sizeof(int)*sz, cntForward, memSz, memOffset, op, pType, 0);
        printf("inci send data! %d %d\n", globalId, 2*(int)(sizeof(int)*sz));
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
            if(replyFieldDes->name() == "VoteMap"){
              const google::protobuf::Reflection* replyRefl = reply->GetReflection();
              google::protobuf::Message * replyField = replyRefl->MutableMessage(reply, replyFdes);
              const google::protobuf::FieldDescriptor* replyFieldFdes = replyFieldDes->field(0); //data field of VoteMap
              assert(replyFieldFdes != nullptr);  // in case of typo or something.
              const google::protobuf::Reflection* replyFieldRefl = replyField->GetReflection();
              for(int i = 0; i<sz; i++){
                google::protobuf::MapKey keys;
                google::protobuf::MapValueRef values;
                int tmp_key = 0, tmp_value = 0;
                memcpy(&tmp_key, data+2*i*sizeof(int), sizeof(int));
                memcpy(&tmp_value, data+(2*i+1)*sizeof(int), sizeof(int));
                keys.SetInt32Value(tmp_key);
                replyFieldRefl->InsertOrLookupMapValue(replyField, replyFieldFdes, keys, &values);
                values.SetInt32Value(tmp_value);
              }
              break;
            }
          }
        }

        free(data);
        sendSucc = true;
        break;
      } else if(requestFieldDes->name() == "CustomMap"){
        const google::protobuf::Reflection* requestRefl = request.GetReflection();
        google::protobuf::Message * requestField = requestRefl->MutableMessage(const_cast<google::protobuf::Message*>(&request), requestFdes);
        const google::protobuf::FieldDescriptor* requestFieldFdes = requestFieldDes->field(0); //data field of AggtrArray
        assert(requestFieldFdes != nullptr);  // in case of typo or something.
        const google::protobuf::Reflection* requestFieldRefl = requestField->GetReflection();
        const google::protobuf::FieldDescriptor* confInfoFdes = requestFieldDes->field(1); //conf field of AggtrArray
        const google::protobuf::Descriptor* confInfoDes = confInfoFdes->message_type();
        const google::protobuf::Message & confInfo = requestFieldRefl->GetMessage(*requestField, confInfoFdes);
        const google::protobuf::Reflection* confInfoRefl = confInfo.GetReflection();
        const google::protobuf::FieldDescriptor* cntForwardFdes = confInfoDes->field(0);
        int cntForward = confInfoRefl->GetInt32(confInfo, cntForwardFdes);
        const google::protobuf::FieldDescriptor* pTypeFdes = confInfoDes->field(1); 
        int pType = confInfoRefl->GetInt32(confInfo, pTypeFdes);
        const google::protobuf::FieldDescriptor* flagFdes = confInfoDes->field(3); 
        int flag = confInfoRefl->GetInt32(confInfo, flagFdes);
        printf("flag:%d\n", flag);

        int sz = requestFieldRefl->FieldSize(*requestField, requestFieldFdes);
        assert(sz>0);

        if(hasInit == false)initService(6,pType,2*sz*sizeof(int),1,2000,0);
        else assert(op==6);

        char* data;
        data = (char*)malloc(sz*sizeof(int)*2);
        auto it = requestFieldRefl->MapBegin(requestField, requestFieldFdes);
        for(int i = 0; i<sz; i++){
          int tmp_key = it.GetKey().GetInt32Value();
          memcpy(data+2*i*sizeof(int), &tmp_key, sizeof(int));
          int tmp_value = it.GetValueRef().GetInt32Value();
          memcpy(data+(2*i+1)*sizeof(int), &tmp_value, sizeof(int));
          it++;
        }

        comm::MsgArgs *args = createArgs(globalId, 2*sizeof(int)*sz, cntForward, memSz, memOffset, op, pType, flag);
        printf("inci send data! %d %d\n", globalId, 2*(int)(sizeof(int)*sz));
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
            if(replyFieldDes->name() == "CustomMap"){
              const google::protobuf::Reflection* replyRefl = reply->GetReflection();
              google::protobuf::Message * replyField = replyRefl->MutableMessage(reply, replyFdes);
              const google::protobuf::FieldDescriptor* replyFieldFdes = replyFieldDes->field(0); //data field of VoteMap
              assert(replyFieldFdes != nullptr);  // in case of typo or something.
              const google::protobuf::Reflection* replyFieldRefl = replyField->GetReflection();
              for(int i = 0; i<sz; i++){
                google::protobuf::MapKey keys;
                google::protobuf::MapValueRef values;
                int tmp_key = 0, tmp_value = 0;
                memcpy(&tmp_key, data+2*i*sizeof(int), sizeof(int));
                memcpy(&tmp_value, data+(2*i+1)*sizeof(int), sizeof(int));
                keys.SetInt32Value(tmp_key);
                replyFieldRefl->InsertOrLookupMapValue(replyField, replyFieldFdes, keys, &values);
                values.SetInt32Value(tmp_value);
              }
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
  const google::protobuf::Descriptor* replyDes = reply->GetDescriptor();
  int replyFieldNum = replyDes->field_count();
  for(int i = 0; i<replyFieldNum; i++){
    const google::protobuf::FieldDescriptor* replyFdes = replyDes->field(i);
    assert(replyFdes != nullptr);  // in case of typo or something.
    if(replyFdes->type()==google::protobuf::FieldDescriptor::TYPE_MESSAGE){
      const google::protobuf::Descriptor* replyFieldDes = replyFdes->message_type();
      printf("inci field type in reply: %s\n", replyFieldDes->name().c_str());
      if(replyFieldDes->name() == "QueryArray"){
        const google::protobuf::Reflection* replyRefl = reply->GetReflection();
        google::protobuf::Message * replyField = replyRefl->MutableMessage(reply, replyFdes);
        const google::protobuf::FieldDescriptor* replyFieldFdes = replyFieldDes->field(0); //data field of AggtrArray
        assert(replyFieldFdes != nullptr);  // in case of typo or something.
        const google::protobuf::Reflection* replyFieldRefl = replyField->GetReflection();
        auto replyArray = replyFieldRefl->MutableRepeatedField<int>(replyField, replyFieldFdes);

        // query to get data
        char* data;
        int sz = (int)(32*memSz*sizeof(int)) > dataLength?dataLength:(int)(32*memSz*sizeof(int));
        data = (char*)calloc(1, sz);
        comm::MsgArgs *args = createArgs(globalId, sz, 0, memSz, memOffset, op, pType, 5);
        printf("inci query data! %d %d\n", globalId, (int)(sz));
        client->PushPull(data, args);
        free(args);

        replyArray->Resize(sz/sizeof(int), 0);
        memcpy(&((*replyArray)[0]), data, sz);
        break;
      }
    }
  }
  return sendSucc;
}

}