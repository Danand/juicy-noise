#!/bin/bash

find . -name "juicy-noise-fx.so" \
| while read -r so_file; do
  platform="$(basename "$(dirname "${so_file}")")"

  archive_name="juicy-noise-fx-0.1.3-${platform}"
  archive_dir="${archive_name}/juicy-noise-fx.vst3/Contents/${platform}"

  mkdir -p "${archive_dir}"

  cp "${so_file}" "${archive_dir}/"

  zip -r "${archive_name}.zip" "${archive_dir}/"

  done
