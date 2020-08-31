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
try {
    ftp_fget($ftp, STDOUT, 'x', 0);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    ftp_fput($ftp, 'x', fopen(__FILE__, 'r'), 0);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    ftp_get($ftp, 'x', 'y', 0);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(ftp_mdtm($ftp, 'x'));
var_dump(ftp_mkdir($ftp, 'x'));
var_dump(ftp_nb_continue($ftp));

try {
    ftp_nb_fget($ftp, STDOUT, 'x', 0);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    ftp_nb_fput($ftp, 'x', fopen(__FILE__, 'r'), 0);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(ftp_systype($ftp));
var_dump(ftp_pwd($ftp));
var_dump(ftp_size($ftp, ''));
var_dump(ftp_rmdir($ftp, ''));

?>
--EXPECTF--
bool(true)
bool(false)

Warning: ftp_cdup(): Command not implemented (1). in %s005.php on line %d
bool(false)

Warning: ftp_chdir(): Command not implemented (2). in %s005.php on line %d
bool(false)

Warning: ftp_chmod(): Command not implemented (3). in %s005.php on line %d
bool(false)

Warning: ftp_delete(): Command not implemented (4). in %s005.php on line %d
bool(false)

Warning: ftp_exec(): Command not implemented (5). in %s005.php on line %d
bool(false)
ftp_fget(): Argument #4 ($mode) must be either FTP_ASCII or FTP_BINARY
ftp_fput(): Argument #4 ($mode) must be either FTP_ASCII or FTP_BINARY
ftp_get(): Argument #4 ($mode) must be either FTP_ASCII or FTP_BINARY
int(-1)

Warning: ftp_mkdir(): Command not implemented (7). in %s005.php on line %d
bool(false)

Warning: ftp_nb_continue(): No nbronous transfer to continue in %s005.php on line %d
int(0)
ftp_nb_fget(): Argument #4 ($mode) must be either FTP_ASCII or FTP_BINARY
ftp_nb_fput(): Argument #4 ($mode) must be either FTP_ASCII or FTP_BINARY

Warning: ftp_systype(): Command not implemented (8). in %s005.php on line %d
bool(false)

Warning: ftp_pwd(): Command not implemented (9). in %s005.php on line %d
bool(false)
int(-1)

Warning: ftp_rmdir(): Command not implemented (11). in %s005.php on line %d
bool(false)
