package(default_visibility = ["//visibility:public"])


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

