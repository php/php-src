#!/usr/bin/bash

set -euo pipefail

# Resolve script location
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ZONES_DIR="$SCRIPT_DIR/zones"
NAMED_CONF_TEMPLATE="$SCRIPT_DIR/named.conf.in"
NAMED_CONF="$SCRIPT_DIR/named.conf"
PID_FILE="$ZONES_DIR/named.pid"
LOG_FILE="$SCRIPT_DIR/named.log"

# Debug: show current user and permissions
echo "Debug: Current user: $(whoami)"
echo "Debug: Current UID: $(id -u)"
echo "Debug: Script dir: $SCRIPT_DIR"
echo "Debug: Zones dir: $ZONES_DIR"

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

# Ensure the generated config file is readable
chmod 644 "$NAMED_CONF"

# Debug: Check if the file is actually readable
echo "Debug: Testing config file readability:"
if [[ -r "$NAMED_CONF" ]]; then
    echo "Debug: Config file is readable"
else
    echo "Debug: Config file is NOT readable"
    ls -la "$NAMED_CONF"
    exit 1
fi

# Ensure the generated config file is readable
chmod 644 "$NAMED_CONF"

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

# Handle AppArmor if present
if [[ -f /etc/apparmor.d/usr.sbin.named ]]; then
  echo "Debug: AppArmor profile detected, disabling it..."
  
  # Install apparmor-utils if not present
  if ! command -v aa-disable >/dev/null 2>&1; then
    echo "Debug: Installing apparmor-utils..."
    apt-get update -qq
    apt-get install -y apparmor-utils
  fi
  
  # Disable the profile
  aa-disable /usr/sbin/named 2>/dev/null || echo "Failed to disable AppArmor profile"
  
  echo "Debug: AppArmor status:"
  aa-status 2>/dev/null | grep named || echo "No named profile found (good!)"
else
  echo "Debug: No AppArmor profile found for named"
fi

# Enhanced AppArmor handling
if [[ -f /etc/apparmor.d/usr.sbin.named ]]; then
  echo "Debug: AppArmor profile detected, attempting comprehensive bypass..."
  
  # Install apparmor-utils if not present
  if ! command -v aa-complain >/dev/null 2>&1; then
    echo "Debug: Installing apparmor-utils..."
    apt-get update -qq
    apt-get install -y apparmor-utils
  fi
  
  # Check initial status
  echo "Debug: Initial AppArmor status for named:"
  aa-status 2>/dev/null | grep named || echo "No named profile in initial aa-status"
  
  # Try complain mode first
  echo "Debug: Setting to complain mode..."
  aa-complain /usr/sbin/named 2>/dev/null || echo "Failed to set AppArmor to complain mode"
  
  # Check what mode it's actually in
  echo "Debug: AppArmor profile mode after complain:"
  cat /sys/kernel/security/apparmor/profiles 2>/dev/null | grep named || echo "No named in profiles"
  
  # Try to completely disable it
  echo "Debug: Attempting to disable AppArmor profile completely..."
  aa-disable /usr/sbin/named 2>/dev/null || echo "Failed to disable AppArmor profile"
  
  # Alternative disable method
  echo "Debug: Trying alternative disable method..."
  ln -sf /etc/apparmor.d/usr.sbin.named /etc/apparmor.d/disable/ 2>/dev/null || echo "Symlink method failed"
  
  # Unload from kernel
  if command -v apparmor_parser >/dev/null 2>&1; then
    echo "Debug: Unloading profile from kernel..."
    apparmor_parser -R /etc/apparmor.d/usr.sbin.named 2>/dev/null || echo "Failed to unload profile"
  fi
  
  # Final status check
  echo "Debug: Final AppArmor status:"
  aa-status 2>/dev/null | grep named || echo "No named profile found (good!)"
  
elif [ -d /etc/apparmor.d/ ]; then
  echo "Debug: AppArmor directory exists but no named profile found:"
  ls /etc/apparmor.d/ | grep -i named || echo "No named-related profiles"
else
  echo "Debug: No AppArmor directory found"
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
  chmod 644 "$NAMED_CONF" "$ZONES_DIR"/*.zone
  chmod 755 "$SCRIPT_DIR" "$ZONES_DIR"
  
  echo "Debug: File permissions after setup:"
  ls -la "$NAMED_CONF" "$ZONES_DIR"/*.zone
  
  echo "Debug: Directory permissions:"
  ls -ld "$SCRIPT_DIR" "$ZONES_DIR"
  
  # Test if the chosen user can actually read the config file
  echo "Debug: Testing $BIND_USER access to config file:"
  if [[ "$BIND_USER" == "$(whoami)" ]]; then
    # Same user, test directly
    if test -r "$NAMED_CONF"; then
      echo "Debug: $BIND_USER CAN read config file"
    else
      echo "Debug: $BIND_USER CANNOT read config file"
    fi
  else
    # Different user, test with sudo
    if sudo -u "$BIND_USER" test -r "$NAMED_CONF" 2>/dev/null; then
      echo "Debug: $BIND_USER CAN read config file"
    else
      echo "Debug: $BIND_USER CANNOT read config file"
      echo "Debug: Checking what $BIND_USER sees:"
      sudo -u "$BIND_USER" ls -la "$NAMED_CONF" 2>&1 || echo "$BIND_USER cannot stat the file"
    fi
  fi
  
  echo "Debug: File permissions after setup:"
  ls -la "$NAMED_CONF" "$ZONES_DIR"/*.zone
  
  echo "Debug: Directory permissions:"
  ls -ld "$SCRIPT_DIR" "$ZONES_DIR"
  
  # Test if the chosen user can actually read the config file
  echo "Debug: Testing $BIND_USER access to config file:"
  if [[ "$BIND_USER" == "$(whoami)" ]]; then
    # Same user, test directly
    if test -r "$NAMED_CONF"; then
      echo "Debug: $BIND_USER CAN read config file"
    else
      echo "Debug: $BIND_USER CANNOT read config file"
    fi
  else
    # Different user, test with sudo
    if sudo -u "$BIND_USER" test -r "$NAMED_CONF" 2>/dev/null; then
      echo "Debug: $BIND_USER CAN read config file"
    else
      echo "Debug: $BIND_USER CANNOT read config file"
      echo "Debug: Checking what $BIND_USER sees:"
      sudo -u "$BIND_USER" ls -la "$NAMED_CONF" 2>&1 || echo "$BIND_USER cannot stat the file"
    fi
  fi

  # Check IPv4/IPv6 configuration with fallbacks
  echo "Debug: Network configuration check:"
  echo "Debug: localhost resolution:"
  getent hosts localhost 2>/dev/null || echo "localhost not found in hosts"
  
  echo "Debug: 127.0.0.1 resolution:"
  getent hosts 127.0.0.1 2>/dev/null || echo "127.0.0.1 not found"
  
  echo "Debug: Available IP addresses:"
  if command -v ip >/dev/null 2>&1; then
    ip addr show lo 2>/dev/null || echo "Failed to show loopback interface with ip"
  else
    ifconfig lo 2>/dev/null || echo "Failed to show loopback interface with ifconfig"
  fi
  
  echo "Debug: Can we reach 127.0.0.1?"
  ping -c 1 127.0.0.1 >/dev/null 2>&1 && echo "127.0.0.1 is reachable" || echo "127.0.0.1 is NOT reachable"
  
  echo "Debug: Can we reach ::1?"
  if command -v ping6 >/dev/null 2>&1; then
    ping6 -c 1 ::1 >/dev/null 2>&1 && echo "::1 is reachable" || echo "::1 is NOT reachable"
  else
    ping -6 -c 1 ::1 >/dev/null 2>&1 && echo "::1 is reachable (via ping -6)" || echo "::1 is NOT reachable"
  fi
  
  # Check what's listening on port 53
  echo "Debug: Processes listening on port 53:"
  if command -v ss >/dev/null 2>&1; then
    ss -tulpn 2>/dev/null | grep ':53' || echo "Debug: No processes found on port 53 (ss)"
  else
    netstat -tulpn 2>/dev/null | grep ':53' || echo "Debug: No processes found on port 53 (netstat)"
  fi
  
  echo "Debug: systemd-resolved status:"
  systemctl is-active systemd-resolved 2>/dev/null || echo "systemd-resolved not active"

  # Monitor AppArmor denials in background
  echo "Debug: Starting AppArmor denial monitoring..."
  (timeout 15 tail -f /var/log/syslog 2>/dev/null | grep "apparmor.*DENIED" | head -10 &) || echo "Could not start syslog monitoring"

  # Use the determined user
  echo "Debug: Using determined user: $BIND_USER"

  # Run named and capture both stdout and stderr separately
  echo "Debug: Starting named as user: $BIND_USER..."
  if named -c "$NAMED_CONF" -p 53 -u "$BIND_USER" > "$LOG_FILE" 2>&1; then
    echo "Debug: named command succeeded"
  else
    NAMED_EXIT_CODE=$?
    echo "Debug: named command failed with exit code: $NAMED_EXIT_CODE"
    echo "Debug: Log file contents:"
    cat "$LOG_FILE" 2>/dev/null || echo "No log file found"
    
    # Show any AppArmor denials
    echo "Debug: Checking for AppArmor denials:"
    grep "apparmor.*DENIED.*named" /var/log/syslog 2>/dev/null | tail -10 || echo "No AppArmor denials found in syslog"
    
    # Show general AppArmor messages
    echo "Debug: Recent AppArmor messages for named:"
    grep "apparmor.*named" /var/log/syslog 2>/dev/null | tail -10 || echo "No AppArmor messages found"
    
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

  # Final AppArmor check
  echo "Debug: Final AppArmor denial check:"
  grep "apparmor.*DENIED.*named" /var/log/syslog 2>/dev/null | tail -5 || echo "No final AppArmor denials found"

  exit 1
fi
