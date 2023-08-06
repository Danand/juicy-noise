#!/bin/bash
#
# Installs artifact on Android device via ADB.

set -e

script_dir="$(dirname "$(realpath "${BASH_SOURCE}")")"

artifacts_dir="$(realpath "${script_dir}/../artifacts")"

apk_file="$( \
  find "${artifacts_dir}/apk" \
    -type f \
    -name "*.apk" \
)"

adb shell pm uninstall "com.danand.juicynoise"
adb install "${apk_file}"
