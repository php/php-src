#!/usr/bin/bash

set -euo pipefail

# Resolve script location
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ZONES_DIR="$SCRIPT_DIR/zones"
NAMED_CONF_TEMPLATE="$SCRIPT_DIR/named.conf.in"
NAMED_CONF="$SCRIPT_DIR/named.conf"
PID_FILE="$ZONES_DIR/named.pid"
LOG_FILE="$SCRIPT_DIR/named.log"

# Default mode: background
FOREGROUND=false
if [[ "${1:-}" == "-f" ]]; then
  FOREGROUND=true
fi

# Ensure zones directory exists
if [ ! -d "$ZONES_DIR" ]; then
  echo "Zone directory $ZONES_DIR not found."
  exit 1
fi

# Ensure template exists
if [ ! -f "$NAMED_CONF_TEMPLATE" ]; then
  echo "Template file $NAMED_CONF_TEMPLATE not found."
  exit 1
fi

# Generate named.conf from template
echo "Generating $NAMED_CONF from $NAMED_CONF_TEMPLATE"
sed -e "s|@ZONES_DIR@|$ZONES_DIR|g" \
    -e "s|@PID_FILE@|$PID_FILE|g" \
    -e "s|@SCRIPT_DIR@|$SCRIPT_DIR|g" \
    "$NAMED_CONF_TEMPLATE" > "$NAMED_CONF"

# Clean up any leftover journal or PID files
rm -f "$ZONES_DIR"/*.jnl "$PID_FILE"

# Print what we're doing
echo "Starting BIND from $SCRIPT_DIR"

if $FOREGROUND; then
  echo "(running in foreground)"
  exec named -c "$NAMED_CONF" -p 53 -u "$(whoami)" -g -d 1
else
  echo "(running in background)"
  named -c "$NAMED_CONF" -p 53 -u "$(whoami)"

  # Wait for BIND to start with periodic checks
  MAX_WAIT=20  # Maximum wait time in attempts (20 * 0.5s = 10s)
  CHECK_INTERVAL=0.5  # Check every 500ms
  ATTEMPTS=0

  echo -n "Waiting for BIND to start"

  while [[ $ATTEMPTS -lt $MAX_WAIT ]]; do
    if [[ -f "$PID_FILE" ]] && kill -0 "$(cat "$PID_FILE")" 2>/dev/null; then
      echo ""  # New line after the dots
      ELAPSED=$(echo "scale=1; $ATTEMPTS * $CHECK_INTERVAL" | bc 2>/dev/null || echo "${ATTEMPTS}")
      echo "BIND started in background with PID $(cat "$PID_FILE") (took ~${ELAPSED}s)"
      exit 0
    fi
    
    echo -n "."
    sleep "$CHECK_INTERVAL"
    ((ATTEMPTS++))
  done

  echo ""  # New line after the dots
  TOTAL_WAIT=$(echo "scale=1; $MAX_WAIT * $CHECK_INTERVAL" | bc 2>/dev/null || echo "${MAX_WAIT}")
  echo "Failed to start BIND within ~${TOTAL_WAIT}s. See $LOG_FILE for details."

  # Show last few lines of log for debugging
  if [[ -f "$LOG_FILE" ]]; then
    echo "Last few lines from log:"
    tail -5 "$LOG_FILE"
  fi

  exit 1
fi
