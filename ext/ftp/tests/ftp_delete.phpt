--TEST--
Testing ftp_delete basic functionality
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
Contributed by Ward Cappelle <wardcappelle@gmail.com>
User Group: PHP-WVL & PHPGent #PHPTestFest
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
ftp_login($ftp, 'user', 'pass');
$ftp or die("Couldn't connect to the server");

echo "Test case #1: removal of existing file from FTP, should return true:", PHP_EOL;
var_dump(ftp_delete($ftp, 'file1'));

echo "Test case #2: removal of non-existent file from FTP, should return false:", PHP_EOL;
var_dump(ftp_delete($ftp, 'false-file.boo'));

ftp_close($ftp);
?>
--EXPECTF--
Test case #1: removal of existing file from FTP, should return true:
bool(true)
Test case #2: removal of non-existent file from FTP, should return false:

Warning: ftp_delete(): No such file or directory in %s on line %d
bool(false)
