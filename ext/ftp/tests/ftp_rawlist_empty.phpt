--TEST--
Testing ftp_rawlist empty directory
--CREDITS--
Chance Garcia <chance [at] chancegarcia [dot] com>
#testfest php[tek] 2018-06-01
--FILE--
<?php

require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
if (!$ftp) die("Couldn't connect to the server");

var_dump(ftp_login($ftp, 'user', 'pass'));

var_dump(ftp_rawlist($ftp, ''));
var_dump(ftp_rawlist($ftp, 'emptydir'));

ftp_close($ftp);

?>
--EXPECT--
bool(true)
string(6) "LIST
"
bool(false)
bool(false)