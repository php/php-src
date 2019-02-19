--TEST--
FTP with bogus parameters
--SKIPIF--
<?php
require 'skipif.inc';
?>
--FILE--
<?php
require 'server.inc';

// Negative timeout
var_dump(ftp_connect('127.0.0.1', 0, -3));

$ftp = ftp_connect('127.0.0.1', $port);
if (!$ftp) die("Couldn't connect to the server");

var_dump(ftp_login($ftp, 'user', 'pass'));
var_dump(ftp_login($ftp, 'user', 'bogus'));

var_dump(ftp_quit($ftp));
?>
--EXPECTF--
Warning: ftp_connect(): Timeout has to be greater than 0 in %s on line %d
bool(false)
bool(true)

Warning: ftp_login(): Not logged in. in %s on line %d
bool(false)
bool(true)
