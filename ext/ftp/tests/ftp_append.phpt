--TEST--
ftp_append() create new file and append something
--SKIPIF--
<?php
require 'skipif.inc';
?>
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
if (!$ftp) die("Couldn't connect to the server");

var_dump(ftp_login($ftp, 'user', 'pass'));

file_put_contents(__DIR__ . '/ftp_append_foo', 'foo');
var_dump(ftp_append($ftp, 'ftp_append_foo', 'ftp_append_foo', FTP_BINARY));

file_put_contents(__DIR__ . '/ftp_append_bar', 'bar');
var_dump(ftp_append($ftp, 'ftp_append_bar', 'ftp_append_bar', FTP_BINARY));

ftp_close($ftp);
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
