#!/usr/bin/bash

set -euo pipefail

# Resolve script location
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ZONES_DIR="$SCRIPT_DIR/zones"
PID_FILE="$ZONES_DIR/named.pid"
NAMED_CONF="$SCRIPT_DIR/named.conf"

if [ -f "$PID_FILE" ]; then
  NAMED_PID=$(cat $PID_FILE)
  if [ -n "$NAMED_PID" ]; then
    echo "Stopping BIND running on pid $NAMED_PID"
    kill $NAMED_PID
  else
    echo "BIND pid is empty"
  fi
else
  echo "BIND is not running"
fi

if [ -f "$NAMED_CONF" ]; then
  rm "$NAMED_CONF"
fi