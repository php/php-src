--TEST--
Testing ftp_rename error conditions
--CREDITS--
Chance Garcia <chance [at] chancegarcia [dot] com>
#testfest php[tek] 2018-06-01
--FILE--
<?php

$notFtp = fopen('php://stdin', 'r+');
ftp_rename($notFtp, 'some_file.txt.bak', 'some_file.txt');

?>
--EXPECTF--
Warning: ftp_rename(): supplied resource is not a valid FTP Buffer resource in %s on line %d