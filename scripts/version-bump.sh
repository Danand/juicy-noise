#!/bin/bash
#
# Bumps patch version of project.

set -e

script_dir="$(dirname "$(realpath "${BASH_SOURCE}")")"

tag_latest="$(git describe --tags --abbrev=0)"

patch_old="$(echo "${tag_latest}" | awk -F '.' '{ print $NF }')"
patch_new="$(( $patch_old + 1 ))"
major_minor="$(echo "${tag_latest}" | awk -F '.' '{ print $1"."$2 }')"

tag_new="${major_minor}.${patch_new}"

version_code_latest="$( \
  cat "${script_dir}/../juicy-noise-android/app/build.gradle" \
  | grep -o "\s*versionCode\s*\d*" \
  | awk -F ' ' '{ print $NF }' \
)"

version_code_new=$(( version_code_latest + 1 ))

export VERSION_LAST="${tag_latest}"
export VERSION_NEXT="${tag_new}"
export VERSION_CODE_LAST="${version_code_latest}"
export VERSION_CODE_NEXT="${version_code_new}"

bump_git_patch_template="${script_dir}/../patches/bump-template.patch"
bump_git_patch="bump-${tag_new}.patch"

envsubst '${VERSION_LAST} ${VERSION_NEXT} ${VERSION_CODE_LAST} ${VERSION_CODE_NEXT}' \
< "${bump_git_patch_template}" \
> "${bump_git_patch}"

git apply \
  "${bump_git_patch}" \
  --unidiff-zero \
  --index \
  --verbose

rm -f "${bump_git_patch}"

git commit -m "Bump version to ${tag_new}"

git tag -f "${tag_new}"
