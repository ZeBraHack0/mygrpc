/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <memory>
#include <string>
#include <stdlib.h>

#include <grpcpp/grpcpp.h>

#include "inci_client.h"
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

void InciConfClient::Configure(int op, int ptype, int dataLength, ConfReply* reply) {
  // Data we are sending to the server.
  ConfRequest request;
  request.set_datalength(dataLength);
  // request.set_name(user);

  // Container for the data we expect from the server.
  request.set_datalength(dataLength);
  request.set_op(op);
  request.set_ptype(ptype);

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The actual RPC.
  Status status = stub_->Configure(&context, request, reply);

  // Act upon its status.
  if (status.ok()) {
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
  }
}

}

