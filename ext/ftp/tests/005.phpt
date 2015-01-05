--TEST--
FTP with bogus server responses
--SKIPIF--
<?php
require 'skipif.inc';
?>
--FILE--
<?php
$bogus = 1;
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
if (!$ftp) die("Couldn't connect to the server");

var_dump(ftp_login($ftp, 'anonymous', 'mail@example.com'));

var_dump(ftp_alloc($ftp, 400));
var_dump(ftp_cdup($ftp));
var_dump(ftp_chdir($ftp, '~'));
var_dump(ftp_chmod($ftp, 0666, 'x'));
var_dump(ftp_delete($ftp, 'x'));
var_dump(ftp_exec($ftp, 'x'));
var_dump(ftp_fget($ftp, STDOUT, 'x', 0));
var_dump(ftp_fput($ftp, 'x', fopen(__FILE__, 'r'), 0));
var_dump(ftp_get($ftp, 'x', 'y', 0));
var_dump(ftp_mdtm($ftp, 'x'));
var_dump(ftp_mkdir($ftp, 'x'));
var_dump(ftp_nb_continue($ftp));
var_dump(ftp_nb_fget($ftp, STDOUT, 'x', 0));
var_dump(ftp_nb_fput($ftp, 'x', fopen(__FILE__, 'r'), 0));
var_dump(ftp_systype($ftp));
var_dump(ftp_pwd($ftp));
var_dump(ftp_size($ftp, ''));
var_dump(ftp_rmdir($ftp, ''));

?>
--EXPECTF--
bool(true)
bool(false)

Warning: ftp_cdup(): Command not implemented (1). in %s005.php on line 11
bool(false)

Warning: ftp_chdir(): Command not implemented (2). in %s005.php on line 12
bool(false)

Warning: ftp_chmod(): Command not implemented (3). in %s005.php on line 13
bool(false)

Warning: ftp_delete(): Command not implemented (4). in %s005.php on line 14
bool(false)

Warning: ftp_exec(): Command not implemented (5). in %s005.php on line 15
bool(false)

Warning: ftp_fget(): Mode must be FTP_ASCII or FTP_BINARY in %s005.php on line 16
bool(false)

Warning: ftp_fput(): Mode must be FTP_ASCII or FTP_BINARY in %s005.php on line 17
bool(false)

Warning: ftp_get(): Mode must be FTP_ASCII or FTP_BINARY in %s005.php on line 18
bool(false)
int(-1)

Warning: ftp_mkdir(): Command not implemented (7). in %s005.php on line 20
bool(false)

Warning: ftp_nb_continue(): no nbronous transfer to continue. in %s005.php on line 21
int(0)

Warning: ftp_nb_fget(): Mode must be FTP_ASCII or FTP_BINARY in %s005.php on line 22
bool(false)

Warning: ftp_nb_fput(): Mode must be FTP_ASCII or FTP_BINARY in %s005.php on line 23
bool(false)

Warning: ftp_systype(): Command not implemented (8). in %s005.php on line 24
bool(false)

Warning: ftp_pwd(): Command not implemented (9). in %s005.php on line 25
bool(false)
int(-1)

Warning: ftp_rmdir(): Command not implemented (11). in %s005.php on line 27
bool(false)
