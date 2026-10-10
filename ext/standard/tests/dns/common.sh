TEST_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ZONES_DIR="$TEST_DIR/zones"
NAMED_CONF="$TEST_DIR/named.conf"
PID_FILE="$TEST_DIR/named.pid"
LOG_FILE="$TEST_DIR/named.log"

DNS_ADDRESS="${PHP_DNS_TEST_ADDRESS:-127.0.0.1}"
DNS_PORT="${PHP_DNS_TEST_PORT:-53}"

RESOLV_CONF="/etc/resolv.conf"
RESOLV_CONF_BACKUP="/etc/resolv.conf.php-dns-test.orig"

AA_PROFILE="/etc/apparmor.d/usr.sbin.named"

fail() {
  echo "$*" >&2
  exit 1
}

resolv_nameservers() {
  [[ -r "$1" ]] && sed -nE 's/^[[:space:]]*nameserver[[:space:]]+([^[:space:]#]+).*/\1/p' "$1"
  return 0
}

# Nameservers the host uses, skipping loopback (the systemd-resolved stub or our own server)
upstream_nameservers() {
  local f ns found=0
  for f in "$RESOLV_CONF_BACKUP" /run/systemd/resolve/resolv.conf "$RESOLV_CONF"; do
    while read -r ns; do
      case "$ns" in
        127.*|::1) ;;
        *) echo "$ns"; found=1 ;;
      esac
    done < <(resolv_nameservers "$f")
    [[ $found -eq 1 ]] && return 0
  done
}

running_pid() {
  local pid
  pid="$(cat "$PID_FILE" 2>/dev/null)" || return 1
  [[ -n "$pid" ]] && kill -0 "$pid" 2>/dev/null || return 1
  echo "$pid"
}

can_manage_apparmor() {
  [[ -f "$AA_PROFILE" && -d /sys/kernel/security/apparmor && "$(id -u)" -eq 0 ]] \
    && command -v apparmor_parser >/dev/null
}
