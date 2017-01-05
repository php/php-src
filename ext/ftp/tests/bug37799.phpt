--TEST--
Bug #37799 (ftp_ssl_connect() falls back to non-ssl connection)
--SKIPIF--
<?php
$ssl = 1;
require 'skipif.inc';
if (!function_exists("ftp_ssl_connect")) die("skip ftp_ssl is disabled");
?>
--FILE--
<?php
$bug37799=$ssl=1;
require 'server.inc';

$ftp = ftp_ssl_connect('127.0.0.1', $port);
if (!$ftp) die("Couldn't connect to the server");

var_dump(ftp_login($ftp, 'user', 'pass'));

ftp_close($ftp);
?>
--EXPECTF--
Warning: ftp_login(): %rdummy|bogus msg%r in %sbug37799.php on line 8
bool(false)
