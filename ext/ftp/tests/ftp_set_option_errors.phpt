--TEST--
Testing ftp_set_option erros while setting up
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

var_dump(ftp_set_option($ftp, FTP_TIMEOUT_SEC, 0));
var_dump(ftp_set_option($ftp, FTP_TIMEOUT_SEC, '0'));
var_dump(ftp_set_option($ftp, FTP_USEPASVADDRESS, ['1']));
var_dump(ftp_set_option($ftp, FTP_AUTOSEEK, 'true'));
var_dump(ftp_set_option($ftp, FOO_BAR, 1));
?>
--EXPECTF--
Warning: ftp_set_option(): Timeout has to be greater than 0 in %s on line %d
bool(false)

Warning: ftp_set_option(): Option TIMEOUT_SEC expects value of type int, string given in %s on line %d
bool(false)

Warning: ftp_set_option(): Option USEPASVADDRESS expects value of type bool, array given in %s on line %d
bool(false)

Warning: ftp_set_option(): Option AUTOSEEK expects value of type bool, string given in %s on line %d
bool(false)

Warning: ftp_set_option(): Unknown option '10' in %s on line %d
bool(false)
