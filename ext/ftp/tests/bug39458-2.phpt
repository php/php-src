--TEST--
Bug #39458 (ftp_nlist() returns false on empty directories (other server behaviour))
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
$bug39458=1;
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
if (!$ftp) die("Couldn't connect to the server");

var_dump(ftp_login($ftp, 'user', 'pass'));

var_dump(ftp_nlist($ftp, ''));
var_dump(ftp_nlist($ftp, 'emptydir'));
var_dump(ftp_nlist($ftp, 'bogusdir'));

ftp_close($ftp);
?>
--EXPECT--
bool(true)
array(3) {
  [0]=>
  string(5) "file1"
  [1]=>
  string(5) "file1"
  [2]=>
  string(9) "file
b0rk"
}
array(0) {
}
bool(false)
