--TEST--
ftp_nlist() without the optional parameter $directory (defaults to ".")
--SKIPIF--
<?php
require 'skipif.inc';
?>
--FILE--
<?php
$bug39458=1;
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
if (!$ftp) die("Couldn't connect to the server");

var_dump(ftp_login($ftp, 'user', 'pass'));
var_dump(ftp_nlist($ftp));
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
