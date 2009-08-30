--TEST--
Testing ftp_nb_fget ignore autoresume if autoseek is switched off
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
ftp_set_option($ftp, FTP_AUTOSEEK, false);

$local_file = dirname(__FILE__) . DIRECTORY_SEPARATOR . "localfile.txt";
$handle = fopen($local_file, 'w');

var_dump(ftp_nb_fget($ftp, $handle, 'fget.txt', FTP_ASCII, FTP_AUTORESUME));
var_dump(file_get_contents($local_file));
?>
--CLEAN--
<?php
@unlink(dirname(__FILE__) . DIRECTORY_SEPARATOR . "localfile.txt");
?>
--EXPECT--
int(2)
string(12) "ASCIIFooBar
"
