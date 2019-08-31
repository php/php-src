--TEST--
Testing ftp_fget autoresume
--CREDITS--
Rodrigo Moyle <eu [at] rodrigorm [dot] com [dot] br>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php
require 'skipif.inc';
?>
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
ftp_login($ftp, 'user', 'pass');
if (!$ftp) die("Couldn't connect to the server");

$local_file = __DIR__ . DIRECTORY_SEPARATOR . "ftp_fget_basic2.txt";
file_put_contents($local_file, 'ASCIIFoo');
$handle = fopen($local_file, 'a');

var_dump(ftp_fget($ftp, $handle, 'fgetresume.txt', FTP_ASCII, FTP_AUTORESUME));
var_dump(file_get_contents($local_file));
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . "ftp_fget_basic2.txt");
?>
--EXPECT--
bool(true)
string(12) "ASCIIFooBar
"
