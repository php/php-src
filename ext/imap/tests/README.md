# The imap extension tests

Many of the tests in this directory require a mail server to be running, if
there is no mail server the test will skip and warn, see skipif.inc for details.

To make the tests run parameters in the `skipif.inc` and `imap_include.inc`
files will need to be changed to match the local mailserver configuration.

The tests have been checked using dovecot (on Linux 32 and 64 bit systems) and
hMailServer on Windows. The tests are intended to be mailserver agnostic.

## Set-up tests on Ubuntu (checked on Ubuntu 18.04 (Bionic))
The necessary packages can be installed using the following command;
`apt-get install libc-client-dev libkrb5-dev dovecot-core dovecot-pop3d dovecot-imapd sendmail`

The build of PHP will need to be compiled with the following flags:
```
--with-imap --with-kerberos --with-imap-ssl
```

Then run the set-up script `ext/imap/tests/setup/setup.sh` which will add the `vmail`
group and user which is used by Dovecot for the mailbox. It will also copy the
`ext/imap/tests/setup/dovecot.conf` and `ext/imap/tests/setup/dovecotpass` to the correct
location for Dovecot and restarts it for the new configuration to be enabled.
