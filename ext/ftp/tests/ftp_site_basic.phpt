--TEST--
ftp_site function basic functionality
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
$ftp or die("Couldn't connect to the server");

ftp_login($ftp, 'user', 'pass') or die("Couldn't login into the server");

var_dump(ftp_site($ftp, 'CHMOD 0600 file'));
var_dump(ftp_site($ftp, 'foo bar baz'));
?>
--EXPECTF--
bool(true)

Warning: ftp_site(): Syntax error, command unrecognized. in %s on line %d
bool(false)
