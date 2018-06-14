--TEST--
Testing ftp_cdup error conditions
--CREDITS--
Chance Garcia <chance [at] chancegarcia [dot] com>
#testfest php[tek] 2018-06-01
--FILE--
<?php

$notFtp = fopen('php://stdin', 'r+');
ftp_cdup($notFtp);

?>
--EXPECTF--
Warning: ftp_cdup(): supplied resource is not a valid FTP Buffer resource in %s on line %d