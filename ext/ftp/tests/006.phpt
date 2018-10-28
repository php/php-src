--TEST--
FTP with bogus parameters
--SKIPIF--
<?php
require 'skipif.inc';
?>
--FILE--
<?php
$ftp=null;

var_dump(ftp_connect(array()));
var_dump(ftp_connect('127.0.0.1', 0, -3));
var_dump(ftp_raw($ftp));
var_dump(ftp_mkdir($ftp));
var_dump(ftp_rmdir($ftp));
var_dump(ftp_nlist($ftp));
var_dump(ftp_rawlist($ftp));
var_dump(ftp_fget($ftp));
var_dump(ftp_nb_fget($ftp));
var_dump(ftp_nb_get($ftp));
var_dump(ftp_pasv($ftp));
var_dump(ftp_nb_continue());
var_dump(ftp_fput());
var_dump(ftp_nb_fput($ftp));
var_dump(ftp_put($ftp));
var_dump(ftp_nb_put($ftp));
var_dump(ftp_size($ftp));
var_dump(ftp_mdtm($ftp));
var_dump(ftp_rename($ftp));
var_dump(ftp_site($ftp));
var_dump(ftp_set_option($ftp));
var_dump(ftp_get_option($ftp));
var_dump(ftp_mlsd($ftp));
var_dump(ftp_append($ftp));

?>
--EXPECTF--
Warning: ftp_connect() expects parameter 1 to be string, array given in %s006.php on line 4
NULL

Warning: ftp_connect(): Timeout has to be greater than 0 in %s006.php on line 5
bool(false)

Warning: ftp_raw() expects exactly 2 parameters, 1 given in %s006.php on line 6
NULL

Warning: ftp_mkdir() expects exactly 2 parameters, 1 given in %s006.php on line 7
NULL

Warning: ftp_rmdir() expects exactly 2 parameters, 1 given in %s006.php on line 8
NULL

Warning: ftp_nlist() expects exactly 2 parameters, 1 given in %s006.php on line 9
NULL

Warning: ftp_rawlist() expects at least 2 parameters, 1 given in %s006.php on line 10
NULL

Warning: ftp_fget() expects at least 3 parameters, 1 given in %s006.php on line 11
NULL

Warning: ftp_nb_fget() expects at least 3 parameters, 1 given in %s006.php on line 12
NULL

Warning: ftp_nb_get() expects at least 3 parameters, 1 given in %s006.php on line 13
NULL

Warning: ftp_pasv() expects exactly 2 parameters, 1 given in %s006.php on line 14
NULL

Warning: ftp_nb_continue() expects exactly 1 parameter, 0 given in %s006.php on line 15
NULL

Warning: ftp_fput() expects at least 3 parameters, 0 given in %s006.php on line 16
NULL

Warning: ftp_nb_fput() expects at least 3 parameters, 1 given in %s006.php on line 17
NULL

Warning: ftp_put() expects at least 3 parameters, 1 given in %s006.php on line 18
NULL

Warning: ftp_nb_put() expects at least 3 parameters, 1 given in %s006.php on line 19
NULL

Warning: ftp_size() expects exactly 2 parameters, 1 given in %s006.php on line 20
NULL

Warning: ftp_mdtm() expects exactly 2 parameters, 1 given in %s006.php on line 21
NULL

Warning: ftp_rename() expects exactly 3 parameters, 1 given in %s006.php on line 22
NULL

Warning: ftp_site() expects exactly 2 parameters, 1 given in %s006.php on line 23
NULL

Warning: ftp_set_option() expects exactly 3 parameters, 1 given in %s006.php on line 24
NULL

Warning: ftp_get_option() expects exactly 2 parameters, 1 given in %s006.php on line 25
NULL

Warning: ftp_mlsd() expects exactly 2 parameters, 1 given in %s006.php on line 26
NULL

Warning: ftp_append() expects at least 3 parameters, 1 given in %s006.php on line 27
NULL
