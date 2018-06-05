--TEST--
Testing ftp_alloc error conditions
--CREDITS--
Chance Garcia <chance [at] chancegarcia [dot] com>
#testfest php[tek] 2018-06-01
--FILE--
<?php

$notFtp = fopen('php://stdin', 'r+');
ftp_alloc($notFtp, 1, $result);

?>
--EXPECTF--
Warning: ftp_alloc(): supplied resource is not a valid FTP Buffer resource in %s on line %d