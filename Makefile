SHELL = /bin/bash

.SHELLFLAGS = -ec

.ONESHELL:
.SILENT:

OS = "linux"
PROTOBUF_VERSION = "23.2"
GRPC_VERSION = "1.55.0"
BAZELISK_VERSION = "1.17.0"

BUILD_DIR = $(shell pwd)/build

GRPC_REPO_URL = "https://github.com/grpc/grpc"
GRPC_REPO_DIR = "$(BUILD_DIR)/grpc"
GRPC_INSTALL_DIR = "$(BUILD_DIR)/grpc-installed"

PROTOBUF_REPO_URL = "https://github.com/protocolbuffers/protobuf.git"
PROTOBUF_REPO_DIR = "$(BUILD_DIR)/protobuf"
PROTOBUF_INSTALL_DIR = "$(BUILD_DIR)/protobuf/bazel-bin"

PROTO_SOURCE_DIR = "$(shell pwd)/juicy-noise-proto"
PROTO_COMPILED_DIR = "$(shell pwd)/juicy-noise-fx/Source/Generated/Proto"

BAZELISK_DOWNLOAD_URL = "https://github.com/bazelbuild/bazelisk/releases/download/v$(BAZELISK_VERSION)/bazelisk-$(OS)-amd64"
BAZEL_PATH = "/usr/local/bin/bazel"

DEPS_LOCK_FILE = "$(BUILD_DIR)/deps.lock"

IS_ROOT := $$(if [ $$(id -u) -eq 0 ]; then echo "true"; else echo "false"; fi)
ELEVATION := $$(if $(IS_ROOT); then echo ""; else echo "sudo"; fi)

$(BUILD_DIR):
	mkdir -p "$@"

$(DEPS_LOCK_FILE): $(BUILD_DIR)
	$(ELEVATION) \
	apt update
	$(ELEVATION) \
	apt install -y \
		wget \
	  build-essential \
	  autoconf \
	  libtool \
	  pkg-config \
	  g++
	\
	touch "$@"

install-bazel: $(BAZEL_PATH)

$(BAZEL_PATH):
	filename="bazel"
	wget \
	  --tries=0 \
	  --retry-connrefused \
	  $(BAZELISK_DOWNLOAD_URL) \
	  -O "$${filename}"
	$(ELEVATION) mv "$${filename}" "$@"
	$(ELEVATION) chmod +x "$@"

compile-proto: $(PROTO_COMPILED_DIR)

$(PROTO_COMPILED_DIR): #$(PROTOBUF_INSTALL_DIR) $(GRPC_INSTALL_DIR)
	rm -rf "$@"
	mkdir -p "$@"
	\
	$(PROTOBUF_INSTALL_DIR)/protoc \
	  --proto_path="$(PROTO_SOURCE_DIR)" \
	  --cpp_out="$@" \
	  --grpc_out="$@" \
	  --plugin=protoc-gen-grpc=$(GRPC_INSTALL_DIR)/bin/grpc_cpp_plugin \
	  $$(find "$(PROTO_SOURCE_DIR)" -type f -name "*.proto" -print0 | xargs -0)

$(PROTOBUF_REPO_DIR): $(BUILD_DIR)
	git clone \
	  --recurse-submodules \
	  --branch "v$(PROTOBUF_VERSION)" \
	  --depth 1 \
	  --shallow-submodules \
	  "$(PROTOBUF_REPO_URL)" \
	  "$@"

install-protobuf: $(PROTOBUF_INSTALL_DIR)

$(PROTOBUF_INSTALL_DIR): $(BAZEL_PATH) $(PROTOBUF_REPO_DIR)
	$(SHELL) -c 'cd $(PROTOBUF_REPO_DIR) && $< build :protoc :protobuf'  \

$(GRPC_REPO_DIR): $(BUILD_DIR)
	git clone \
	  --recurse-submodules \
	  --branch "v$(GRPC_VERSION)" \
	  --depth 1 \
	  --shallow-submodules \
	  "$(GRPC_REPO_URL)" \
	  "$@"

install-grpc: $(GRPC_INSTALL_DIR)

$(GRPC_INSTALL_DIR): #$(DEPS_LOCK_FILE) $(GRPC_REPO_DIR)
	build_dir="$(GRPC_REPO_DIR)/cmake/build"
	\
	mkdir -p "$${build_dir}"
	mkdir -p "$@"
	\
	( \
	  cd "$${build_dir}" \
	  && \
	  cmake \
	    "-DgRPC_INSTALL=ON" \
	    "-DgRPC_BUILD_TESTS=OFF" \
	    "-DCMAKE_INSTALL_PREFIX='$@'" \
	    "../.." \
	  && \
	  make -j 4 \
	  && \
	  make install \
	)
