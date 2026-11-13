#!/usr/bin/env bash
# Points /etc/resolv.conf to the BIND started by bind-start.sh. The PHP DNS
# functions use libresolv which reads this file directly, so it is replaced
# instead of configuring systemd-resolved or NetworkManager. The current
# nameservers are kept as a fallback and resolv-reset.sh restores the file.

set -euo pipefail

source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

[[ "$(id -u)" -eq 0 ]] || fail "must run as root"
[[ "$DNS_PORT" == "53" ]] || fail "resolv.conf cannot use port $DNS_PORT"

if [[ -e "$RESOLV_CONF_BACKUP" || -L "$RESOLV_CONF_BACKUP" ]]; then
  echo "$RESOLV_CONF_BACKUP exists, already set up"
  exit 0
fi

content="nameserver $DNS_ADDRESS"$'\n'
while read -r ns; do
  [[ "$ns" == "$DNS_ADDRESS" ]] || content+="nameserver $ns"$'\n'
done < <(resolv_nameservers "$RESOLV_CONF" | head -n 2)

# In a container /etc/resolv.conf is a bind mount that can only be written in place
cp -P "$RESOLV_CONF" "$RESOLV_CONF_BACKUP"
if [[ -L "$RESOLV_CONF" ]]; then
  rm -f "$RESOLV_CONF"
fi
printf '%s' "$content" > "$RESOLV_CONF"
chmod 644 "$RESOLV_CONF"

echo "$RESOLV_CONF now contains:"
cat "$RESOLV_CONF"
