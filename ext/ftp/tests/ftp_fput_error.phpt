--TEST--
Testing ftp_fput error conditions
--CREDITS--
Chance Garcia <chance [at] chancegarcia [dot] com>
#testfest php[tek] 2018-06-01
--FILE--
<?php

$notFtp = fopen('php://stdin', 'r+');
$destination_file = basename(__FILE__);
$source_file = fopen(__FILE__, 'r');
ftp_fput($notFtp, $destination_file, $source_file, FTP_ASCII);

?>
--EXPECTF--
Warning: ftp_fput(): supplied resource is not a valid FTP Buffer resource in %s on line %d