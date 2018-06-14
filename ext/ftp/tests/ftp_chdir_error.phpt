--TEST--
Testing ftp_chdir error conditions
--CREDITS--
Chance Garcia <chance [at] chancegarcia [dot] com>
#testfest php[tek] 2018-06-01
--FILE--
<?php

$notFtp = fopen('php://stdin', 'r+');
ftp_chdir($notFtp, '/path/to/some/dir');

?>
--EXPECTF--
Warning: ftp_chdir(): supplied resource is not a valid FTP Buffer resource in %s on line %d