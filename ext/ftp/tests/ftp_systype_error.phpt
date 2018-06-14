--TEST--
Testing ftp_systype error conditions
--CREDITS--
Chance Garcia <chance [at] chancegarcia [dot] com>
#testfest php[tek] 2018-06-01
--FILE--
<?php

$notFtp = fopen('php://stdin', 'r+');
ftp_systype($notFtp);

?>
--EXPECTF--
Warning: ftp_systype(): supplied resource is not a valid FTP Buffer resource in %s on line %d