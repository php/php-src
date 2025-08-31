# The snmp extension tests

To enable these tests, you must have:

* PHP compiled with SNMP `--with-snmp`
* An SNMP server running.

## How to test

You need to give credentials with environment vars if default ones are not
suitable (see `snmp_include.inc` for more info):

```txt
SNMP_HOSTNAME : IPv4 of remote SNMP agent
SNMP_HOSTNAME : IPv6 or remote SNMP agent
SNMP_PORT : SNMP port for queries
SNMP_COMMUNITY : community name
SNMP_COMMUNITY_WRITE : community used for write tests (snmpset()).
SNMP_MIBDIR : Directory containing MIBS
```

To run test suite you may use this command (presuming that you pwd is where this
README file is located):

```bash
make -C ../../.. test TESTS="`cd ../../..; /bin/ls -1 ext/snmp/tests/*.phpt | xargs echo`"
```

Running `run-tests.php` directly will clear your environment and therefore tests
will fail if your SNMP configuration does not fit into default values specified
in `snmp_include.inc`.

## Configuring the SNMPD server

### On Linux/FreeBSD

* On Ubuntu, install `snmpd` and `snmp-mibs-downloader`. (May be `net-snmpd`
  on other distributions.)
* Replace config file (by default this is `/etc/snmp/snmpd.conf` on Linux and
  `/usr/local/etc/snmp/snmpd.conf` on FreeBSD) with `snmpd.conf` supplied.

Before launching daemon make sure that there is no file
`/var/net-snmp/snmpd.conf`. Delete it if exists. Forgetting to do so will fail
SNMPv3 tests.

* Place `ext/snmp/tests/bigtest` near `snmpd.conf`, tune path to it in
  `snmpd.conf`.
* Launch snmpd (service snmpd start or /etc/init.d/snmpd start). Alternatively
  you can start snmpd daemon using following command line:

    ```bash
    sudo snmpd -C -c ./snmpd.conf -f -Le
    ```
