sudo service dovecot stop
sudo groupadd -g 5000 vmail
sudo useradd -m -d /var/vmail -s /bin/false -u 5000 -g vmail vmail
sudo cp ext/imap/tests/setup/dovecot.conf /etc/dovecot/dovecot.conf
sudo cp ext/imap/tests/setup/dovecotpass /etc/dovecot/dovecotpass
sudo service dovecot start
