--TEST--
Testing ftp_set_option errors while setting up
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php
require 'server.inc';
define('FOO_BAR', 10);

$ftp = ftp_connect('127.0.0.1', $port);
ftp_login($ftp, 'user', 'pass');
$ftp or die("Couldn't connect to the server");

try {
    ftp_set_option($ftp, FTP_TIMEOUT_SEC, 0);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    ftp_set_option($ftp, FTP_TIMEOUT_SEC, '0');
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    ftp_set_option($ftp, FTP_USEPASVADDRESS, ['1']);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    ftp_set_option($ftp, FTP_AUTOSEEK, 'true');
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    ftp_set_option($ftp, FOO_BAR, 1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
ftp_set_option(): Argument #3 ($value) must be greater than 0 for the FTP_TIMEOUT_SEC option
ftp_set_option(): Argument #3 ($value) must be of type int for the FTP_TIMEOUT_SEC option, string given
ftp_set_option(): Argument #3 ($value) must be of type bool for the FTP_USEPASVADDRESS option, array given
ftp_set_option(): Argument #3 ($value) must be of type bool for the FTP_AUTOSEEK option, string given
ftp_set_option(): Argument #2 ($option) must be one of FTP_TIMEOUT_SEC, FTP_AUTOSEEK, or FTP_USEPASVADDRESS
