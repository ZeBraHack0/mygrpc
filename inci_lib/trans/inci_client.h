#ifndef _INCICLIENT_H_
#define _INCICLIENT_H_

#include <iostream>
#include <memory>
#include <string>
#include <stdlib.h>

#include <grpcpp/grpcpp.h>

#include "proto/inci.grpc.pb.h"

#define TENSOR_SIZE 10000000

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using inci::InciConf;
using inci::ConfReply;
using inci::ConfRequest;


// comm::MsgArgs* createArgs(int key, int dataLength, int worker_num, int mem_sz, int mem_offset){
//   comm::MsgArgs *args = (comm::MsgArgs *)malloc(sizeof(comm::MsgArgs));
//   memcpy(args->margs, &key, sizeof(int));
//   memcpy(args->margs+sizeof(int), &dataLength, sizeof(int));
//   memcpy(args->margs+2*sizeof(int), &worker_num, sizeof(int));
//   memcpy(args->margs+3*sizeof(int), &mem_sz, sizeof(int));
//   memcpy(args->margs+4*sizeof(int), &mem_offset, sizeof(int));
//   return args;
// }

namespace inciclient {

class InciConfClient {
 public:
  InciConfClient(std::shared_ptr<Channel> channel)
      : stub_(InciConf::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  void Configure(int op, int ptype, int dataLength, ConfReply* reply);

 private:
  std::unique_ptr<InciConf::Stub> stub_;
};

}

#endif //INCICLIENT