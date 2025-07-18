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

# Check if 127.0.0.1 is available and decide on listen address
echo "Debug: Testing network connectivity for BIND address selection..."

IPV4_OK=false
IPV6_OK=false

# Test IPv4 connectivity
if ping -c 1 127.0.0.1 >/dev/null 2>&1; then
  IPV4_OK=true
  echo "Debug: IPv4 (127.0.0.1) is reachable"
else
  echo "Debug: IPv4 (127.0.0.1) is NOT reachable"
fi

# Test IPv6 connectivity
if command -v ping6 >/dev/null 2>&1; then
  if ping6 -c 1 ::1 >/dev/null 2>&1; then
    IPV6_OK=true
    echo "Debug: IPv6 (::1) is reachable"
  fi
else
  if ping -6 -c 1 ::1 >/dev/null 2>&1; then
    IPV6_OK=true
    echo "Debug: IPv6 (::1) is reachable via ping -6"
  fi
fi

if ! $IPV6_OK; then
  echo "Debug: IPv6 (::1) is NOT reachable"
fi

# Choose the listen address
if $IPV4_OK; then
  LISTEN_ADDRESS="127.0.0.1"
  echo "Debug: Using IPv4 (127.0.0.1) for BIND"
elif $IPV6_OK; then
  LISTEN_ADDRESS="::1"
  echo "Debug: Using IPv6 (::1) for BIND"
else
  echo "Debug: Neither 127.0.0.1 nor ::1 is available!"
  echo "Debug: Falling back to 127.0.0.1 anyway"
  LISTEN_ADDRESS="127.0.0.1"
fi

sed -e "s|@ZONES_DIR@|$ZONES_DIR|g" \
    -e "s|@PID_FILE@|$PID_FILE|g" \
    -e "s|@SCRIPT_DIR@|$SCRIPT_DIR|g" \
    -e "s|@LISTEN_ADDRESS@|$LISTEN_ADDRESS|g" \
    "$NAMED_CONF_TEMPLATE" > "$NAMED_CONF"

# Determine the best user to run BIND as (do this early)
echo "Debug: Determining user for BIND..."

# Get the owner of the script directory
SCRIPT_OWNER=$(stat -c '%U' "$SCRIPT_DIR")
SCRIPT_GROUP=$(stat -c '%G' "$SCRIPT_DIR")

echo "Debug: Script directory owned by: $SCRIPT_OWNER:$SCRIPT_GROUP"
echo "Debug: Current user: $(whoami)"

# Use the script owner if it's not root, otherwise use current user
if [[ "$SCRIPT_OWNER" != "root" ]] && id "$SCRIPT_OWNER" >/dev/null 2>&1; then
  BIND_USER="$SCRIPT_OWNER"
  echo "Debug: Will run BIND as script owner: $BIND_USER"
else
  BIND_USER="$(whoami)"
  echo "Debug: Will run BIND as current user: $BIND_USER"
fi

echo "Debug: Generated named.conf contents:"
cat "$NAMED_CONF"

# Clean up any leftover journal or PID files
rm -f "$ZONES_DIR"/*.jnl "$PID_FILE"

# Print what we're doing
echo "Starting BIND from $SCRIPT_DIR"

if $FOREGROUND; then
  echo "(running in foreground)"
  echo "Debug: About to exec: named -c $NAMED_CONF -p 53 -u $BIND_USER -g -d 1"
  exec named -c "$NAMED_CONF" -p 53 -u "$BIND_USER" -g -d 1
else
  echo "(running in background)"
  echo "Debug: About to run: named -c $NAMED_CONF -p 53 -u $BIND_USER"
  
  # Test configuration first
  echo "Debug: Testing BIND configuration..."
  if named-checkconf "$NAMED_CONF"; then
    echo "Debug: Configuration check passed"
  else
    echo "Debug: Configuration check failed"
    exit 1
  fi
  
  # Check if zone files exist
  echo "Debug: Checking zone files..."
  if [[ -f "$ZONES_DIR/basic.dnstest.php.net.zone" ]]; then
    echo "Debug: Zone file exists"
    echo "Debug: Zone file contents:"
    cat "$ZONES_DIR/basic.dnstest.php.net.zone"
  else
    echo "Debug: Zone file missing: $ZONES_DIR/basic.dnstest.php.net.zone"
    ls -la "$ZONES_DIR/"
    exit 1
  fi

  # Set up permissions for the chosen user
  echo "Debug: Setting up permissions for user: $BIND_USER..."
  
  # Ensure files are readable by the chosen user
  if [[ "$BIND_USER" != "$(whoami)" ]]; then
    # If we're running as a different user, ensure group/other permissions
    chmod 644 "$NAMED_CONF" "$ZONES_DIR"/*.zone
    chmod 755 "$SCRIPT_DIR" "$ZONES_DIR"
  fi

  # Run named and capture both stdout and stderr separately
  echo "Debug: Starting named as user: $BIND_USER..."
  if named -c "$NAMED_CONF" -p 53 -u "$BIND_USER" > "$LOG_FILE" 2>&1; then
    echo "Debug: named command succeeded"
  else
    NAMED_EXIT_CODE=$?
    echo "Debug: named command failed with exit code: $NAMED_EXIT_CODE"
    echo "Debug: Log file contents:"
    cat "$LOG_FILE" 2>/dev/null || echo "No log file found"
    
    # Try to run named with more verbose output
    echo "Debug: Trying to run named in foreground for better error output:"
    timeout 5 named -c "$NAMED_CONF" -p 53 -u "$BIND_USER" -g -d 1 || echo "Foreground attempt timed out or failed"
    
    exit $NAMED_EXIT_CODE
  fi

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
  else
    echo "No log file found at $LOG_FILE"
  fi

  exit 1
fi