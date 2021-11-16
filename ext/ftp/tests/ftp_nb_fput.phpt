--TEST--
Testing ftp_nb_fput basic functionality
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

$destination_file = basename(__FILE__);
$source_file = fopen(__FILE__, 'r');

var_dump(ftp_nb_fput($ftp, $destination_file, $source_file, FTP_ASCII));
?>
--EXPECT--
int(1)
