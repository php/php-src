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
try {
    ftp_connect('127.0.0.1', 0, -3);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

$ftp = ftp_connect('127.0.0.1', $port);
if (!$ftp) die("Couldn't connect to the server");

var_dump(ftp_login($ftp, 'user', 'pass'));
var_dump(ftp_login($ftp, 'user', 'bogus'));

var_dump(ftp_quit($ftp));
?>
--EXPECTF--
ftp_connect(): Argument #3 ($timeout) must be greater than 0
bool(true)

Warning: ftp_login(): Not logged in. in %s on line %d
bool(false)
bool(true)
