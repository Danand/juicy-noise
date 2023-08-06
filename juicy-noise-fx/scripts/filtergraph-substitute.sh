#!/bin/bash
#
# Substitutes filtergraph preset file
# into AudioPluginHost for seamless project debugging.

set -e

filtergraph_file="$1"
plugin_host_settings_file_template="$2"

filtergraph_file_tmp="tmp-${filtergraph_file}"

export PROJECT_DIR="$(pwd)"

envsubst '${PROJECT_DIR}' \
< "${filtergraph_file}" \
> "${filtergraph_file_tmp}"

FILTERGRAPH_PATH="$(realpath "${filtergraph_file_tmp}")"
export FILTERGRAPH_PATH

plugin_host_settings_file="${HOME}/.Juce Audio Plugin Host/Juce Audio Plugin Host.settings"

mkdir -p "$(dirname "${plugin_host_settings_file}")"

envsubst '${FILTERGRAPH_PATH}' \
< "${plugin_host_settings_file_template}" \
> "${plugin_host_settings_file}"
