#!/usr/bin/env bash
# Restores /etc/resolv.conf saved by resolv-setup.sh

set -euo pipefail

source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

[[ "$(id -u)" -eq 0 ]] || fail "must run as root"

if [[ ! -e "$RESOLV_CONF_BACKUP" && ! -L "$RESOLV_CONF_BACKUP" ]]; then
  echo "$RESOLV_CONF_BACKUP not found, nothing to restore"
  exit 0
fi

if [[ -L "$RESOLV_CONF_BACKUP" ]]; then
  rm -f "$RESOLV_CONF"
  mv "$RESOLV_CONF_BACKUP" "$RESOLV_CONF"
else
  cat "$RESOLV_CONF_BACKUP" > "$RESOLV_CONF"
  rm -f "$RESOLV_CONF_BACKUP"
fi

echo "$RESOLV_CONF restored"
