--TEST--
Testing ftp_site error conditions
--CREDITS--
Chance Garcia <chance [at] chancegarcia [dot] com>
#testfest php[tek] 2018-06-01
--FILE--
<?php

$notFtp = fopen('php://stdin', 'r+');
ftp_site($notFtp, 'CHMOD 0600 /home/user/privatefile');

?>
--EXPECTF--
Warning: ftp_site(): supplied resource is not a valid FTP Buffer resource in %s on line %d