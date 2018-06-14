--TEST--
Testing ftp_nb_get error conditions
--CREDITS--
Chance Garcia <chance [at] chancegarcia [dot] com>
#testfest php[tek] 2018-06-01
--FILE--
<?php

$notFtp = fopen('php://stdin', 'r+');
$tmpfname = tempnam(__DIR__, "ftp_test");
ftp_nb_get($notFtp, $tmpfname, 'a story.txt', FTP_ASCII);
unlink($tmpfname);
?>
--EXPECTF--
Warning: ftp_nb_get(): supplied resource is not a valid FTP Buffer resource in %s on line %d