--TEST--
Testing ftp_get_option error conditions
--CREDITS--
Chance Garcia <chance [at] chancegarcia [dot] com>
#testfest php[tek] 2018-06-01
--FILE--
<?php

$notFtp = fopen('php://stdin', 'r+');
ftp_get_option($notFtp, FTP_TIMEOUT_SEC);

?>
--EXPECTF--
Warning: ftp_get_option(): supplied resource is not a valid FTP Buffer resource in %s on line %d