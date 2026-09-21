--TEST--
GH-20601 (ftp_set_option FTP_TIMEOUT_SEC timeout overflow)
--EXTENSIONS--
ftp
pcntl
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip: 64-bit only");
if (PHP_OS_FAMILY === 'Windows') die("skip not for windows");
?>
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
$ftp or die("Couldn't connect to the server");
ftp_login($ftp, 'user', 'pass');

try {
	ftp_set_option($ftp, FTP_TIMEOUT_SEC, PHP_INT_MAX);
} catch (\ValueError $e) {
	echo $e->getMessage();
}
?>
--EXPECTF--
ftp_set_option(): Argument #3 ($value) must be less than %d for the FTP_TIMEOUT_SEC option
