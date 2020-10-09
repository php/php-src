--TEST--
Testing ftp_nb_fget can handle large files incl. resume
--SKIPIF--
<?php
require 'skipif.inc';
if (2147483647 == PHP_INT_MAX) {
    die('skip ot supported on this system');
}
if (disk_free_space(__DIR__) < 10*1024*1024*1024) {
    die('skip Not enough disk space');
}
?>
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
ftp_login($ftp, 'user', 'pass');
if (!$ftp) die("Couldn't connect to the server");

$local_file = __DIR__ . DIRECTORY_SEPARATOR . "ftp_nb_get_large.txt";
touch($local_file);
ftp_nb_get($ftp, $local_file, 'fget_large.txt', FTP_BINARY, 5368709119);
$fp = fopen($local_file, 'r');
fseek($fp, 5368709119);
var_dump(fread($fp, 1));
var_dump(filesize($local_file));
fclose($fp);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . "ftp_nb_get_large.txt");
?>
--EXPECT--
string(1) "X"
int(5368709120)
