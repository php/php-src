# The imap extension tests

Many of the tests in this directory require a mail server to be running, if
there is no mail server the test will skip and warn, see skipif.inc for details.

To make the tests run parameters in the `skipif.inc` and `imap_include.inc`
files will need to be changed to match the local mailserver configuration.

The tests have been checked using dovecot (on Linux 32 and 64 bit systems) and
hMailServer on Windows. The tests are intended to be mailserver agnostic.

The tests can be run without modification with a fairly minimal dovecot
installation. For information, the dovecot.conf file used in running the tests
is given below. The dovecot password file (dovecotpass) requires a password for
one user, `webmaster@something.com`. It's also necessary to set up one
additional user ID (vmail) to own the mail directory.

```txt
protocols = imap imaps

listen = *

ssl_disable = yes

disable_plaintext_auth=yes

## Mailbox locations and namespaces

mail_location = maildir:/home/vmail/mail/%d/%n/Maildir

auth_verbose = yes

auth_debug = yes

auth default {
  mechanisms = login

  passdb passwd-file {
    args = /etc/dovecot/dovecotpass
  }

  userdb static {
    args = uid=11459 gid=1002 home=/home/vmail/dovecot/mail/%d/%n
  }

  user = root
}
```
