--TEST--
Bug #78374 (Warning when closing FTPS connection after FTP_PUT)
--SKIPIF--
<?php
$ssl = 1;
require 'skipif.inc';
if (!function_exists("ftp_ssl_connect")) die("skip ftp_ssl is disabled");
?>
--FILE--
<?php
$ssl = 1;
require 'server.inc';

$ftp = ftp_ssl_connect('127.0.0.1', $port);
if (!$ftp) die("Couldn't connect to the server");

var_dump(ftp_login($ftp, 'user', 'pass'));
var_dump(ftp_put($ftp, basename(__FILE__), __FILE__, FTP_ASCII));
var_dump(ftp_close($ftp));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
