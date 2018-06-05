--TEST--
Testing ftp_mlsd error conditions
--CREDITS--
Chance Garcia <chance [at] chancegarcia [dot] com>
#testfest php[tek] 2018-06-01
--FILE--
<?php

$notFtp = fopen('php://stdin', 'r+');
ftp_mlsd($notFtp, '/path/to/some/dir');

?>
--EXPECTF--
Warning: ftp_mlsd(): supplied resource is not a valid FTP Buffer resource in %s on line %d