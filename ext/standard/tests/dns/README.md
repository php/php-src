# DNS tests

These tests run the PHP DNS functions against a local BIND 9 server serving
the zones from `zones/`. They are skipped unless the server is running and
`/etc/resolv.conf` points to it.

Requirements on Debian / Ubuntu:

```sh
sudo apt-get install bind9 bind9utils bind9-dnsutils
```

Nothing else may listen on `127.0.0.1:53` (the system `named` service started
by the package install needs to be stopped). systemd-resolved listens on
`127.0.0.53` and is not a problem.

Running the tests:

```sh
sudo ext/standard/tests/dns/bind-start.sh
sudo ext/standard/tests/dns/resolv-setup.sh

sapi/cli/php run-tests.php ext/standard/tests/dns

sudo ext/standard/tests/dns/resolv-reset.sh
sudo ext/standard/tests/dns/bind-stop.sh
```

`bind-start.sh` generates `named.conf` from `named.conf.in` and starts `named`
on `127.0.0.1:53` as the owner of this directory. Other queries are forwarded
to the nameservers the host uses so everything else keeps resolving. The
server logs to `named.log`.

`resolv-setup.sh` saves `/etc/resolv.conf` and replaces it with one using
`127.0.0.1` with the previous nameservers as a fallback. The PHP DNS
functions use libresolv which reads this file directly, so this is the only
setting that matters. systemd-resolved is not used on purpose as it re-applies
DHCP servers and routes queries per interface.

To add records, edit `zones/basic.dnstest.php.net.zone` and bump the serial,
or add a new zone file and a `zone` block to `named.conf.in`.
