--TEST--
Testing ftp_append error conditions
--CREDITS--
Chance Garcia <chance [at] chancegarcia [dot] com>
#testfest php[tek] 2018-06-01
--FILE--
<?php

$notFtp = fopen('php://stdin', 'r+');
$destination_file = basename(__FILE__);
ftp_append($notFtp, $destination_file, 'a storty.txt', FTP_ASCII);

?>
--EXPECTF--
Warning: ftp_append(): supplied resource is not a valid FTP Buffer resource in %s on line %d