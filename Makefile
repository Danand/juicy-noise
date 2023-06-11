SHELL = /bin/bash

.SHELLFLAGS = -ec

.ONESHELL:
.SILENT:

PROTOC_VERSION = 23.2
OS = linux

PROTOC_DOWNLOAD_URL := "https://github.com/protocolbuffers/protobuf/releases/download"
PROTOC_DIST := "protoc-$(PROTOC_VERSION)-$(OS)-$$(uname -p)"

.PHONY: \
  build-proto, \
  $(PROTOC_DIST) \

build-proto: | $(PROTOC_DIST)
	mkdir -p "$$(pwd)/juicy-noise-fx/Source/Generated/Proto"
	./$(PROTOC_DIST)/bin/protoc \
	  --proto_path="$$(pwd)/proto" \
	  --cpp_out="$$(pwd)/juicy-noise-fx/Source/Generated/Proto" \
	  $$(find $$(pwd)/proto -type f -name "*.proto" -print0 | xargs -0)

$(PROTOC_DIST):
	if [ ! -d $(PROTOC_DIST) ]; then
	  wget "$(PROTOC_DOWNLOAD_URL)/v$(PROTOC_VERSION)/$(PROTOC_DIST).zip"
	  unzip -o "$(PROTOC_DIST).zip" -d "$(PROTOC_DIST)"
	  rm -f "$(PROTOC_DIST).zip"
	fi
