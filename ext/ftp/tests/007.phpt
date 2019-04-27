--TEST--
FTP with bogus resource
--CREDITS--
Michael Paul da Rosa <michael [at] michaelpaul [dot] com [dot] br>
PHP TestFest Dublin 2017
--SKIPIF--
<?php
require 'skipif.inc';
?>
--FILE--
<?php
$ftp = tmpfile();

var_dump(ftp_login($ftp, 'user', 'pass'));
var_dump(ftp_pwd($ftp));
var_dump(ftp_cdup($ftp));
var_dump(ftp_chdir($ftp, '~'));
var_dump(ftp_exec($ftp, 'x'));
var_dump(ftp_raw($ftp, 'x'));
var_dump(ftp_mkdir($ftp, '/'));
var_dump(ftp_rmdir($ftp, '/'));
var_dump(ftp_chmod($ftp, 7777, '/'));
var_dump(ftp_alloc($ftp, 7777));
var_dump(ftp_nlist($ftp, '/'));
var_dump(ftp_rawlist($ftp, '~'));
var_dump(ftp_mlsd($ftp, '~'));
var_dump(ftp_systype($ftp));
var_dump(ftp_fget($ftp, $ftp, 'remote', 7777));
var_dump(ftp_nb_fget($ftp, $ftp, 'remote', 7777));
var_dump(ftp_pasv($ftp, false));
var_dump(ftp_get($ftp, 'local', 'remote', 7777));
var_dump(ftp_nb_get($ftp, 'local', 'remote', 7777));
var_dump(ftp_nb_continue($ftp));
var_dump(ftp_fput($ftp, 'remote', $ftp, 9999));
var_dump(ftp_nb_fput($ftp, 'remote', $ftp, 9999));
var_dump(ftp_put($ftp, 'remote', 'local', 9999));
var_dump(ftp_append($ftp, 'remote', 'local', 9999));
var_dump(ftp_nb_put($ftp, 'remote', 'local', 9999));
var_dump(ftp_size($ftp, '~'));
var_dump(ftp_mdtm($ftp, '~'));
var_dump(ftp_rename($ftp, 'old', 'new'));
var_dump(ftp_delete($ftp, 'gone'));
var_dump(ftp_site($ftp, 'localhost'));
var_dump(ftp_close($ftp));
var_dump(ftp_set_option($ftp, 1, 2));
var_dump(ftp_get_option($ftp, 1));

fclose($ftp);
?>
--EXPECTF--
Warning: ftp_login(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_pwd(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_cdup(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_chdir(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_exec(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_raw(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_mkdir(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_rmdir(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_chmod(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_alloc(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_nlist(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_rawlist(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_mlsd(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_systype(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_fget(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_nb_fget(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_pasv(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_get(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_nb_get(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_nb_continue(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_fput(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_nb_fput(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_put(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_append(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_nb_put(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_size(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_mdtm(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_rename(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_delete(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_site(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_close(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_set_option(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)

Warning: ftp_get_option(): supplied resource is not a valid FTP Buffer resource in %s007.php on line %d
bool(false)
