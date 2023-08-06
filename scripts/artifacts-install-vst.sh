#!/bin/bash
#
# Installs artifact fot VST host.

set -e

script_dir="$(dirname "$(realpath "${BASH_SOURCE}")")"

artifacts_dir="$(realpath "${script_dir}/../artifacts")"

vst_file_zip="$( \
  find "${artifacts_dir}/vst" \
    -type f \
    -name "*-linux.zip" \
  | head -n 1 \
)"

extracted_dir="${artifacts_dir}/extracted"

unzip \
  -o "${vst_file_zip}" \
  -d "${extracted_dir}" \

vst_dir="$( \
  find "${extracted_dir}" \
    -type d \
    -name "juicy-noise-fx-*" \
)"

build_dir="${script_dir}/../juicy-noise-fx/Builds/LinuxMakefile/build"

output_path="${build_dir}/juicy-noise-fx.vst3"

rm -rf "${output_path}"

mkdir -p "${build_dir}"

cp -rf "${vst_dir}/juicy-noise-fx.vst3" "${output_path}"

echo
echo "Success: ${output_path} was copied into project outputs and ready to run"
echo