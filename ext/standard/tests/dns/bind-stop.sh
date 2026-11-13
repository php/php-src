#!/usr/bin/env bash
# Stops BIND started by bind-start.sh

set -euo pipefail

source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

if pid="$(running_pid)"; then
  kill "$pid"
  for _ in $(seq 1 40); do
    kill -0 "$pid" 2>/dev/null || break
    sleep 0.25
  done
  echo "BIND with PID $pid stopped"
else
  echo "BIND is not running"
fi

rm -f "${PID_FILE:?}" "${NAMED_CONF:?}" "${ZONES_DIR:?}"/*.jnl

if can_manage_apparmor; then
  apparmor_parser -r "$AA_PROFILE" 2>/dev/null || true
fi
