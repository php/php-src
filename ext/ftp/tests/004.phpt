--TEST--
FTP with bogus parameters
--SKIPIF--
<?php
require 'skipif.inc';
?>
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
if (!$ftp) die("Couldn't connect to the server");

var_dump(ftp_login($ftp, 'user', 'pass'));

var_dump(ftp_systype($ftp));

/* some bogus usage */
var_dump(ftp_alloc($ftp, array()));
var_dump(ftp_cdup($ftp, 0));
var_dump(ftp_chdir($ftp, array()));
var_dump(ftp_chmod($ftp, 0666));
var_dump(ftp_close());
var_dump(ftp_connect('sfjkfjaksfjkasjf'));
var_dump(ftp_delete($ftp, array()));
var_dump(ftp_exec($ftp, array()));

var_dump(ftp_systype($ftp, 0));
var_dump(ftp_pwd($ftp, array()));

var_dump(ftp_login($ftp));
var_dump(ftp_login($ftp, 'user', 'bogus'));

var_dump(ftp_quit($ftp));
?>
--EXPECTF--
bool(true)
string(4) "UNIX"

Warning: ftp_alloc() expects parameter 2 to be int, array given in %s004.php on line 12
bool(false)

Warning: ftp_cdup() expects exactly 1 parameter, 2 given in %s004.php on line 13
NULL

Warning: ftp_chdir() expects parameter 2 to be string, array given in %s004.php on line 14
NULL

Warning: ftp_chmod() expects exactly 3 parameters, 2 given in %s on line %d
bool(false)

Warning: ftp_close() expects exactly 1 parameter, 0 given in %s004.php on line 16
NULL

Warning: ftp_connect(): php_network_getaddresses: getaddrinfo failed: %s in %s004.php on line 17
bool(false)

Warning: ftp_delete() expects parameter 2 to be string, array given in %s004.php on line 18
NULL

Warning: ftp_exec() expects parameter 2 to be string, array given in %s004.php on line 19
NULL

Warning: ftp_systype() expects exactly 1 parameter, 2 given in %s004.php on line 21
NULL

Warning: ftp_pwd() expects exactly 1 parameter, 2 given in %s004.php on line 22
NULL

Warning: ftp_login() expects exactly 3 parameters, 1 given in %s004.php on line 24
NULL

Warning: ftp_login(): Not logged in. in %s004.php on line 25
bool(false)
bool(true)
