--TEST--
Testing ftp_close error conditions
--CREDITS--
Chance Garcia <chance [at] chancegarcia [dot] com>
#testfest php[tek] 2018-06-01
--FILE--
<?php

$notFtp = fopen('php://stdin', 'r+');
ftp_close($notFtp);

?>
--EXPECTF--
Warning: ftp_close(): supplied resource is not a valid FTP Buffer resource in %s on line %d