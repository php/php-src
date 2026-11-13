#!/usr/bin/env bash
# Starts BIND serving the zones from zones/ on 127.0.0.1:53, forwarding
# everything else to the nameservers the host uses. Use -f to run it in
# the foreground.

set -euo pipefail

source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

command -v named >/dev/null || fail "named not found, install BIND 9 (bind9 bind9utils bind9-dnsutils)"

if pid="$(running_pid)"; then
  echo "BIND is already running with PID $pid"
  exit 0
fi

# Installing bind9 on Debian/Ubuntu starts the system named on port 53
if command -v ss >/dev/null; then
  listener="$(ss -H -uln "sport = :$DNS_PORT" | awk -v a="$DNS_ADDRESS:$DNS_PORT" -v p="$DNS_PORT" \
    '$4 == a || $4 == "0.0.0.0:" p || $4 == "[::]:" p || $4 == "*:" p { print $4; exit }')"
  [[ -z "$listener" ]] || fail "$listener is already in use, stop the service using it first (e.g. systemctl stop named)"
fi

forwarders=""
while read -r ns; do
  forwarders+="$ns; "
done < <(upstream_nameservers)
if [[ -n "$forwarders" ]]; then
  forwarders="forwarders { $forwarders}; forward first;"
fi

sed -e "s|@TEST_DIR@|$TEST_DIR|g" \
    -e "s|@PID_FILE@|$PID_FILE|g" \
    -e "s|@LOG_FILE@|$LOG_FILE|g" \
    -e "s|@ADDRESS@|$DNS_ADDRESS|g" \
    -e "s|@PORT@|$DNS_PORT|g" \
    -e "s|@FORWARDERS@|$forwarders|" \
    "$TEST_DIR/named.conf.in" > "$NAMED_CONF"
chmod 644 "$NAMED_CONF"

if command -v named-checkconf >/dev/null; then
  named-checkconf -z "$NAMED_CONF" >/dev/null
fi

# Drop privileges to the owner of this directory so named can write here
args=(-c "$NAMED_CONF")
owner=""
if [[ "$(id -u)" -eq 0 ]]; then
  owner="$(stat -c '%U' "$TEST_DIR")"
  if [[ "$owner" != "root" ]]; then
    args+=(-u "$owner")
  fi
elif [[ "$DNS_PORT" -lt 1024 ]]; then
  fail "must run as root to listen on port $DNS_PORT"
fi

# The distribution profile only allows named to read /etc/bind and /var/{cache,lib}/bind
if can_manage_apparmor; then
  apparmor_parser -R "$AA_PROFILE" 2>/dev/null || true
fi

rm -f "${PID_FILE:?}" "${ZONES_DIR:?}"/*.jnl
: > "$LOG_FILE"
[[ -z "$owner" ]] || chown "$owner" "$LOG_FILE"

if [[ "${1:-}" == "-f" ]]; then
  exec named "${args[@]}" -g
fi

named "${args[@]}"

for _ in $(seq 1 40); do
  if pid="$(running_pid)"; then
    if ! command -v dig >/dev/null \
        || [[ "$(dig "@$DNS_ADDRESS" -p "$DNS_PORT" +short +time=1 +tries=1 www.basic.dnstest.php.net A)" == "192.0.2.1" ]]; then
      echo "BIND started with PID $pid on $DNS_ADDRESS:$DNS_PORT"
      exit 0
    fi
  fi
  sleep 0.25
done

cat "$LOG_FILE" >&2
fail "BIND did not start, see $LOG_FILE"
