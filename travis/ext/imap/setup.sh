#!/bin/sh
sudo groupadd -g 10002 vmail
sudo useradd -g 10002 -u 11459 vmail
sudo apt-get -y install expect dovecot-imapd
sudo cp ./travis/ext/imap/dovecot.conf ./travis/ext/imap/dovecotpass /etc/dovecot
sudo mkdir -p /home/vmail/mail/something.com
sudo chown -R vmail:vmail /home/vmail/mail
sudo chmod -R u+w /home/vmail/mail
sudo service dovecot stop
sudo service dovecot start
expect ./travis/ext/imap/imap.exp
