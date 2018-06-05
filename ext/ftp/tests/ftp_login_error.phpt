--TEST--
Testing ftp_login error conditions
--CREDITS--
Chance Garcia <chance [at] chancegarcia [dot] com>
#testfest php[tek] 2018-06-01
--FILE--
<?php

$notFtp = fopen('php://stdin', 'r+');
ftp_login($notFtp, 'user', 'pass');

?>
--EXPECTF--
Warning: ftp_login(): supplied resource is not a valid FTP Buffer resource in %s on line %d