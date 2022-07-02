--TEST--
Verify php can handle filesizes >32bit
--EXTENSIONS--
ftp
pcntl
--SKIPIF--
<?php
if (2147483647 == PHP_INT_MAX) {
    die('skip 64-bit only');
}
?>
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
if (!$ftp) die("Couldn't connect to the server");

ftp_login($ftp, 'user', 'pass');
var_dump(ftp_size($ftp, 'largefile'));

ftp_close($ftp);
?>
--EXPECT--
int(5368709120)
