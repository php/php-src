--TEST--
Testing ftp_mdtm error conditions
--CREDITS--
Chance Garcia <chance [at] chancegarcia [dot] com>
#testfest php[tek] 2018-06-01
--FILE--
<?php

$notFtp = fopen('php://stdin', 'r+');
$destination_file = basename(__FILE__);
ftp_mdtm($notFtp, $destination_file);

?>
--EXPECTF--
Warning: ftp_mdtm(): supplied resource is not a valid FTP Buffer resource in %s on line %d