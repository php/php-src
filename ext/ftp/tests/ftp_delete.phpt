--TEST--
Testing ftp_delete basic functionality
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
ftp_login($ftp, 'user', 'pass');
$ftp or die("Couldn't connect to the server");

var_dump(ftp_delete($ftp, 'file'));
?>
--EXPECT--
bool(true)
