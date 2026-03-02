--TEST--
Testing ftp_set_option basic functionality
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
ftp_login($ftp, 'user', 'pass');
$ftp or die("Couldn't connect to the server");

var_dump(ftp_set_option($ftp, FTP_TIMEOUT_SEC, 10));
var_dump(ftp_set_option($ftp, FTP_AUTOSEEK, false));
var_dump(ftp_set_option($ftp, FTP_USEPASVADDRESS, true));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
