--TEST--
ftp_get()/ftp_nb_get() fail cleanly when the active data connection is never opened
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port, 1);
if (!$ftp) die("Couldn't connect to the server");

var_dump(ftp_login($ftp, 'anonymous', 'IEUser@'));
var_dump(ftp_pasv($ftp, false));

$local_file = __DIR__ . DIRECTORY_SEPARATOR . "ftp_get_data_accept_timeout.txt";

var_dump(ftp_get($ftp, $local_file, 'no_data_connection', FTP_BINARY));
var_dump(ftp_nb_get($ftp, $local_file, 'no_data_connection', FTP_BINARY) === FTP_FAILED);

ftp_close($ftp);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . "ftp_get_data_accept_timeout.txt");
?>
--EXPECTF--
bool(true)
bool(true)

Warning: ftp_get(): %s timed out in %s on line %d

Warning: ftp_get(): File status okay; about to open data connection. in %s on line %d
bool(false)

Warning: ftp_nb_get(): %s timed out in %s on line %d

Warning: ftp_nb_get(): File status okay; about to open data connection. in %s on line %d
bool(true)
