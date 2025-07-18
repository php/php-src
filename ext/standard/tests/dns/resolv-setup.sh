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

# Check if NetworkManager is running
systemctl is-active NetworkManager || echo "NetworkManager disabled"

# Check if systemd-networkd is running  
systemctl is-active systemd-networkd  || echo "systemd-networkd disabled"

# Check what's managing your interface
networkctl status $IFACE

# Get current DNS server
echo "Current configuration:"
resolvectl status "$IFACE" | grep -E 'Current DNS Server:|DNS Servers:'

# Store the original DNS server for fallback
ORIGINAL_DNS=$(resolvectl status "$IFACE" | grep "DNS Servers:" | sed 's/.*DNS Servers: //' | awk '{print $1}')
echo "Original DNS server: $ORIGINAL_DNS"

echo "Setting DNS to $LOCAL_DNS for $IFACE (with fallback to $ORIGINAL_DNS)"

# Reset interface configuration first
resolvectl revert "$IFACE"

# Set DNS with local server FIRST (this makes it primary)
#resolvectl dns "$IFACE" "$LOCAL_DNS" "$ORIGINAL_DNS"
resolvectl dns "$IFACE" "$LOCAL_DNS"

# Flush DNS cache
resolvectl flush-caches

# Confirm setup
echo -e "\nUpdated configuration:"
resolvectl status

# Check again what's managing your interface
networkctl status eth0 

echo -e "\nTesting DNS resolution..."

# Test if our local DNS is working
echo "Testing local BIND server directly:"
if dig @127.0.0.1 www.basic.dnstest.php.net A +short; then
    echo "✓ Local BIND server is responding"
else
    echo "✗ Local BIND server is not responding"
    exit 1
fi

# Test system DNS resolution
echo -e "\nTesting system DNS resolution:"
if dig www.basic.dnstest.php.net A +short; then
    echo "✓ System DNS resolution is working"
else
    echo "✗ System DNS resolution failed"
fi

# Test with nslookup as well
echo -e "\nTesting with nslookup:"
nslookup www.basic.dnstest.php.net || echo "nslookup failed"

# Show which DNS server is actually being used
echo -e "\nFinal verification:"
resolvectl query www.basic.dnstest.php.net || echo "resolvectl query failed"

echo -e "\nDNS configuration has been updated."
