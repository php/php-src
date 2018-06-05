--TEST--
Testing ftp_set_option error conditions
--CREDITS--
Chance Garcia <chance [at] chancegarcia [dot] com>
#testfest php[tek] 2018-06-01
--FILE--
<?php

$notFtp = fopen('php://stdin', 'r+');
ftp_set_option($notFtp, FTP_TIMEOUT_SEC, 120);

?>
--EXPECTF--
Warning: ftp_set_option(): supplied resource is not a valid FTP Buffer resource in %s on line %d