--TEST--
Testing ftp_chmod error conditions
--CREDITS--
Chance Garcia <chance [at] chancegarcia [dot] com>
#testfest php[tek] 2018-06-01
--FILE--
<?php

$notFtp = fopen('php://stdin', 'r+');
ftp_chmod($notFtp, 0000, '/path/to/some/dir');

?>
--EXPECTF--
Warning: ftp_chmod(): supplied resource is not a valid FTP Buffer resource in %s on line %d