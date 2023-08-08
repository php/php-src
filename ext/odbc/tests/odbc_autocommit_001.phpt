--TEST--
odbc_autocommit(): Basic test
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

var_dump(odbc_autocommit($conn, true));
var_dump(odbc_autocommit($conn, null));

var_dump(odbc_autocommit($conn, false));
var_dump(odbc_autocommit($conn));

?>
--EXPECTF--
bool(true)
int(1)
bool(true)
int(0)
