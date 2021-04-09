--TEST--
Testing ftp_set_option errors while setting up
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

$options = [
    [ 'option' => FTP_TIMEOUT_SEC, 'value' => 0 ],
	[ 'option' => FTP_TIMEOUT_SEC, 'value' => '0' ],
	[ 'option' => FTP_USEPASVADDRESS, 'value' => ['1'] ],
	[ 'option' => FTP_AUTOSEEK, 'value' => 'true' ],
	[ 'option' => FOO_BAR, 'value' => 1 ],
];
foreach ($options as $option) try {
	var_dump(ftp_set_option($ftp, $option['option'], $option['value']));
} catch (\Throwable $ex) {
	echo "Exception: ", $ex->getMessage(), "\n";
}

--EXPECT--
Exception: ftp_set_option(): Argument #3 ($value) must be greater than 0 for the FTP_TIMEOUT_SEC option
Exception: ftp_set_option(): Argument #3 ($value) must be of type int for the FTP_TIMEOUT_SEC option, string given
Exception: ftp_set_option(): Argument #3 ($value) must be of type bool for the FTP_USEPASVADDRESS option, array given
Exception: ftp_set_option(): Argument #3 ($value) must be of type bool for the FTP_AUTOSEEK option, string given
Exception: ftp_set_option(): Argument #2 ($option) must be one of FTP_TIMEOUT_SEC, FTP_AUTOSEEK, or FTP_USEPASVADDRESS
