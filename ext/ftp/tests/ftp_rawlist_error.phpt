--TEST--
Testing ftp_rawlist error conditions
--CREDITS--
Chance Garcia <chance [at] chancegarcia [dot] com>
#testfest php[tek] 2018-06-01
--FILE--
<?php

$notFtp = fopen('php://stdin', 'r+');
ftp_rawlist($notFtp, '/path/to/some/dir');

?>
--EXPECTF--
Warning: ftp_rawlist(): supplied resource is not a valid FTP Buffer resource in %s on line %d