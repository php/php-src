--TEST--
ftp_get() ASCII mode: CRLF straddling the FTP_BUFSIZE read boundary
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
ftp_login($ftp, 'user', 'pass');
$ftp or die("Couldn't connect to the server");

$local = __DIR__ . "/crlf_boundary_out.txt";

var_dump(ftp_get($ftp, $local, 'crlf_boundary', FTP_ASCII));
var_dump(file_get_contents($local) === str_repeat("A", 4095) . "\n" . str_repeat("B", 10));
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/crlf_boundary_out.txt");
?>
--EXPECT--
bool(true)
bool(true)
