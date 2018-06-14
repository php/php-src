--TEST--
Testing ftp_exec error conditions
--CREDITS--
Chance Garcia <chance [at] chancegarcia [dot] com>
#testfest php[tek] 2018-06-01
--FILE--
<?php

$notFtp = fopen('php://stdin', 'r+');
ftp_exec($notFtp, 'ls -hal');

?>
--EXPECTF--
Warning: ftp_exec(): supplied resource is not a valid FTP Buffer resource in %s on line %d