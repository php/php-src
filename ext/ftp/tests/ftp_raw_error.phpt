--TEST--
Testing ftp_raw error conditions
--CREDITS--
Chance Garcia <chance [at] chancegarcia [dot] com>
#testfest php[tek] 2018-06-01
--FILE--
<?php

$notFtp = fopen('php://stdin', 'r+');
ftp_raw($notFtp, 'ls -hal');

?>
--EXPECTF--
Warning: ftp_raw(): supplied resource is not a valid FTP Buffer resource in %s on line %d