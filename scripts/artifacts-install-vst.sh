#!/bin/bash
#
# Installs artifact fot VST host.

script_dir="$(dirname "$(realpath "${BASH_SOURCE}")")"

artifacts_dir="${script_dir}/../artifacts"

vst_file_zip="$( \
  find "${artifacts_dir}/vst-linux" \
    -type f \
    -name "*.zip" \
)"

unzip \
  -o "${vst_file_zip}" \
  -d "${artifacts_dir}/extracted" \

vst_dir="$( \
  find "${artifacts_dir}/extracted" \
    -type d \
    -name "juicy-noise-fx-*" \
)"

build_dir="${script_dir}/../juicy-noise-fx/Builds/LinuxMakefile/build"

rm -rf "${build_dir}/juicy-noise-fx.vst3"

mkdir -p "${build_dir}"

cp -rf "${vst_dir}/juicy-noise-fx.vst3" "${build_dir}/juicy-noise-fx.vst3"
