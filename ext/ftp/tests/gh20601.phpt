--TEST--
GH-20601 (ftp_connect timeout overflow)
--EXTENSIONS--
ftp
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip: 64-bit only");
if (PHP_OS_FAMILY === 'Windows') die("skip not for windows");
?>
--FILE--
<?php
try {
	ftp_connect('127.0.0.1', 1024, PHP_INT_MAX);
} catch (\ValueError $e) {
	echo $e->getMessage();
}
?>
--EXPECTF--
ftp_connect(): Argument #3 ($timeout) must be less than %d
