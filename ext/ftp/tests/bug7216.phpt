--TEST--
Bug #7216 (ftp_mkdir returns nothing)
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
$bug7216=true;
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
if (!$ftp) die("Couldn't connect to the server");

var_dump(ftp_login($ftp, 'anonymous', 'IEUser@'));
var_dump(ftp_mkdir($ftp, 'CVS'));

?>
--EXPECT--
bool(true)
string(3) "CVS"
