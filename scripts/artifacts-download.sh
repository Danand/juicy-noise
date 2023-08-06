#!/bin/bash
#
# Downloads latest artifacts via GitHub CLI.

set -e

script_dir="$(dirname "$(realpath "${BASH_SOURCE}")")"

artifacts_dir="$(realpath "${script_dir}/../artifacts")"

rm -rf "${artifacts_dir}"

gh workflow list \
| cut -f 3 \
| while read -r workflow_id; do
    gh run list \
      --json "databaseId,workflowDatabaseId,updatedAt" \
      --status "success" \
    | jq 'sort_by(.updatedAt | strptime("%Y-%m-%dT%H:%M:%SZ") | mktime) | reverse' \
    | jq ".[] | select(.workflowDatabaseId == ${workflow_id}) | .databaseId" \
    | while read -r run_id; do
        gh run download \
          "${run_id}" \
          --dir "${artifacts_dir}" \
        2>/dev/null \
        && ( \
          echo "Downloaded artifacts from:" \
          && gh run view "${run_id}" \
        ) \
        && break \
        || continue
      done
  done
