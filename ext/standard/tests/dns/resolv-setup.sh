#!/usr/bin/bash
set -euo pipefail

LOCAL_DNS="127.0.0.1"

echo "Looking for a DNS-enabled network interface..."

resolvectl status

# Find the interface with DNS and DefaultRoute using grep
IFACE=$(resolvectl status | grep -B1 "Current Scopes: DNS" | grep "Link" | head -n1 | sed -E 's/Link [0-9]+ \(([^)]+)\)/\1/')

if [[ -z "$IFACE" ]]; then
    echo "Could not find a suitable interface with DNS configured."
    exit 1
fi

echo "Using interface: $IFACE"

# Get current DNS server
echo "Current configuration:"
resolvectl status "$IFACE" | grep -E 'Current DNS Server:|DNS Servers:'

echo "Setting DNS to $LOCAL_DNS for $IFACE"

# Reset interface configuration
resolvectl revert "$IFACE"

# Set DNS to local
resolvectl dns "$IFACE" "$LOCAL_DNS"

# Confirm setup
echo -e "\nUpdated configuration:"
resolvectl status "$IFACE" | grep -E 'Current DNS Server:|DNS Servers:'

echo -e "\nDNS configuration has been updated."
