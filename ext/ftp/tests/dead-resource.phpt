--TEST--
Attempt to use a closed FTPConnection
--SKIPIF--
<?php
require 'skipif.inc';
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
if (!$ftp) die("Couldn't connect to the server");
var_dump(ftp_login($ftp, 'user', 'pass'));
var_dump(ftp_close($ftp));

try {
    var_dump(ftp_login($ftp, 'user', 'pass'));
    echo "Login did not throw\n";
} catch (ValueError $ex) {
    echo "Exception: ", $ex->getMessage(), "\n";
}
--EXPECT--
bool(true)
bool(true)
Exception: FTPConnection is already closed
