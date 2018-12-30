--TEST--
ftp_rawlist() without the optional parameter $directory (defaults to ".")
--SKIPIF--
<?php
require 'skipif.inc';
?>
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
ftp_login($ftp, 'user', 'pass');
if (!$ftp) die("Couldn't connect to the server");

var_dump(is_array(ftp_rawlist($ftp)));
?>
--EXPECT--
bool(true)
