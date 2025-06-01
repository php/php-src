#!/usr/bin/bash
set -euo pipefail

echo "Current DNS configuration:"
resolvectl status | grep -E 'Link|Current DNS Server:|DNS Servers:'

echo -e "\nResetting DNS configuration by restarting systemd-resolved..."
systemctl restart systemd-resolved.service

# Give it a moment to fully restart
sleep 1

echo -e "\nUpdated DNS configuration:"
resolvectl status | grep -E 'Link|Current DNS Server:|DNS Servers:'

echo -e "\nDNS configuration has been reset to original state."