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

$fooPath = __DIR__ . '/ftp_append_foo';
file_put_contents($fooPath, 'foo');
var_dump(ftp_append($ftp, 'ftp_append_foobar', $fooPath, FTP_BINARY));

$barPath = __DIR__ . '/ftp_append_bar';
file_put_contents($barPath, 'bar');
var_dump(ftp_append($ftp, 'ftp_append_foobar', $barPath, FTP_BINARY));

$fooBarPath = __DIR__ . '/ftp_append_foobar';
var_dump(file_get_contents($fooBarPath));

ftp_close($ftp);
?>
--CLEAN--
<?php
$fooPath = __DIR__ . '/ftp_append_foo';
unlink($fooPath);
$barPath = __DIR__ . '/ftp_append_bar';
unlink($barPath);
$fooBarPath = __DIR__ . '/ftp_append_foobar';
unlink($fooBarPath);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
string(6) "foobar"
