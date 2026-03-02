--TEST--
Testing ftp_nb_fget ignore autoresume if autoseek is switched off
--CREDITS--
Rodrigo Moyle <eu [at] rodrigorm [dot] com [dot] br>
#testfest PHPSP on 2009-06-20
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
ftp_login($ftp, 'user', 'pass');
if (!$ftp) die("Couldn't connect to the server");
ftp_set_option($ftp, FTP_AUTOSEEK, false);

$local_file = __DIR__ . DIRECTORY_SEPARATOR . "ftp_nb_fget_basic1.txt";
$handle = fopen($local_file, 'w');

var_dump(ftp_nb_fget($ftp, $handle, 'fget.txt', FTP_ASCII, FTP_AUTORESUME));
var_dump(file_get_contents($local_file));
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . "ftp_nb_fget_basic1.txt");
?>
--EXPECT--
int(2)
string(12) "ASCIIFooBar
"
