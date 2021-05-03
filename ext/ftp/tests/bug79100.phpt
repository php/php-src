--TEST--
Bug #79100 (Wrong FTP error messages)
--SKIPIF--
<?php
require 'skipif.inc';
?>
--FILE--
<?php
$bug79100 = true;
require 'server.inc';

$ftp = ftp_connect("127.0.0.1", $port);
if (!$ftp) die("Couldn't connect to the server");
var_dump(ftp_login($ftp, 'user', 'pass'));
var_dump(ftp_set_option($ftp, FTP_TIMEOUT_SEC, 1));
ftp_systype($ftp);
?>
--EXPECTF--
bool(true)
bool(true)

Warning: ftp_systype(): %rConnection|Operation%r timed out in %s on line %d