--TEST--
FTP ftp_fget file for both binary and ASCII transfer modes
--CREDITS--
Nathaniel McHugh
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
if (!$ftp) die("Couldn't connect to the server");

var_dump(ftp_login($ftp, 'user', 'pass'));

//test simple text transfer
$fp = tmpfile();
var_dump(ftp_fget($ftp, $fp ,'a story.txt', FTP_ASCII));
fseek($fp, 0);
echo fgets($fp);

$position = ftell($fp);
//test binary data transfer
var_dump(ftp_fget($ftp, $fp, 'binary data.bin', FTP_BINARY));
fseek($fp, $position);
echo json_encode(fgets($fp)), "\n";

//test non-existent file request
ftp_fget($ftp, $fp ,'a warning.txt', FTP_ASCII);

//remove file
fclose($fp);
?>
--EXPECTF--
bool(true)
bool(true)
For sale: baby shoes, never worn.
bool(true)
"BINARYFoo\u0000Bar\r\n"

Warning: ftp_fget(): a warning: No such file or directory  in %sftp_fget_basic.php on line %d
