--TEST--
FTP ftp_get file for both binary and ASCII transfer modes
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
$tmpfname = tempnam(__DIR__, "ftp_test");
var_dump(ftp_get($ftp, $tmpfname ,'a story.txt', FTP_ASCII));
echo file_get_contents($tmpfname);
unlink($tmpfname);

//test binary data transfer
$tmpfname = tempnam(__DIR__, "ftp_test");
var_dump(ftp_get($ftp, $tmpfname, 'binary data.bin', FTP_BINARY));
echo json_encode(file_get_contents($tmpfname)), "\n";
unlink($tmpfname);

//test non-existent file request
ftp_get($ftp, $tmpfname ,'a warning.txt', FTP_ASCII);
?>
--EXPECTF--
bool(true)
bool(true)
For sale: baby shoes, never worn.
bool(true)
"BINARYFoo\u0000Bar\r\n"

Warning: ftp_get(): a warning: No such file or directory  in %sftp_get_basic.php on line %d
