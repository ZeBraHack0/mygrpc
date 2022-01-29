package(default_visibility = ["//visibility:public"])

cc_library(
    name = "msg_lib",
    srcs = [
        "msg.cc",
    ],
    hdrs = [
        "msg.h",
        ],
)

cc_library(
    name = "comm_lib",
    srcs = [
        "comm.cc",
    ],
    hdrs = [
        "comm.h",
        ],
    deps = [
        "msg_lib",
    ],
    copts = [
        "-std=c++11",
    ],
    linkopts = [
        "-pthread",
    ]
)