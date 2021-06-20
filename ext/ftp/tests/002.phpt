--TEST--
FTP login (SSL)
--EXTENSIONS--
ftp
pcntl
openssl
--SKIPIF--
<?php
if (!function_exists("ftp_ssl_connect")) die("skip ftp_ssl is disabled");
?>
--FILE--
<?php
$ssl = 1;
require 'server.inc';

$ftp = ftp_ssl_connect('127.0.0.1', $port);
if (!$ftp) die("Couldn't connect to the server");

var_dump(ftp_login($ftp, 'user', 'pass'));
var_dump(ftp_raw($ftp, 'HELP'));
var_dump(ftp_raw($ftp, 'HELP HELP'));

var_dump(ftp_close($ftp));
?>
--EXPECT--
bool(true)
array(4) {
  [0]=>
  string(55) "214-There is help available for the following commands:"
  [1]=>
  string(5) " USER"
  [2]=>
  string(5) " HELP"
  [3]=>
  string(15) "214 end of list"
}
array(1) {
  [0]=>
  string(39) "214 Syntax: HELP [<SP> <string>] <CRLF>"
}
bool(true)
