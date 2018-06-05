--TEST--
Testing ftp_fget error conditions
--CREDITS--
Chance Garcia <chance [at] chancegarcia [dot] com>
#testfest php[tek] 2018-06-01
--FILE--
<?php

$notFtp = fopen('php://stdin', 'r+');
$fp = tmpfile();
ftp_fget($notFtp, $fp, 'a story.txt', FTP_ASCII);

?>
--EXPECTF--
Warning: ftp_fget(): supplied resource is not a valid FTP Buffer resource in %s on line %d