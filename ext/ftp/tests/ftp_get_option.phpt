--TEST--
Testing ftp_get_option basic functionality
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
require 'server.inc';
define('FOO_BAR', 10);

$ftp = ftp_connect('127.0.0.1', $port);
ftp_login($ftp, 'user', 'pass');
$ftp or die("Couldn't connect to the server");

var_dump(ftp_get_option($ftp, FTP_TIMEOUT_SEC));
var_dump(ftp_get_option($ftp, FTP_AUTOSEEK));
var_dump(ftp_get_option($ftp, FTP_USEPASVADDRESS));

try {
    ftp_get_option($ftp, FOO_BAR);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECTF--
int(%d)
bool(true)
bool(true)
ftp_get_option(): Argument #2 ($option) must be one of FTP_TIMEOUT_SEC, FTP_AUTOSEEK, or FTP_USEPASVADDRESS
