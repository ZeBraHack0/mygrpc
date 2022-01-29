package(default_visibility = ["//visibility:public"])

cc_library(
    name = "inci_client",
    srcs = ["inci_client.cc"],
    hdrs = ["inci_client.h"],
    defines = ["BAZEL_BUILD"],
    deps = [
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",
        "//proto:inci_grpc",
    ],
)


cc_library(
    name = "inci_stub",
    srcs = ["protoTrans.cc"],
    hdrs = ["protoTrans.h",
            "quantize.h",
    ],
    defines = ["BAZEL_BUILD"],
    deps = [
        "//inci_lib:lib:comm_lib",
    ],
    external_deps = [
        "protobuf_headers",
    ],
)

