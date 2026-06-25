--TEST--
ftp_get() ASCII mode: bare CR is preserved, CRLF folds to LF
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
ftp_login($ftp, 'user', 'pass');
$ftp or die("Couldn't connect to the server");

$local = __DIR__ . "/bare_cr_out.txt";

$expected = "line1\nba\rre\nend" . str_repeat("X", 4078) . "\r" . str_repeat("Y", 10);

var_dump(ftp_get($ftp, $local, 'bare_cr', FTP_ASCII));
var_dump(file_get_contents($local) === $expected);

var_dump(ftp_get($ftp, $local, 'trailing_cr', FTP_ASCII));
var_dump(file_get_contents($local) === "trail\r");
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/bare_cr_out.txt");
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
