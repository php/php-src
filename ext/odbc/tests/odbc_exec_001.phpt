--TEST--
odbc_exec(): Basic test
--EXTENSIONS--
odbc
--SKIPIF--
<?php
include 'skipif.inc';
if ("unixODBC" != ODBC_TYPE) {
    die("skip ODBC_TYPE != unixODBC");
}
?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'foo');

odbc_exec($conn, '');

odbc_exec($conn, 1);

?>
--EXPECTF--
Warning: odbc_exec(): SQL error: %s in %s on line %d

Warning: odbc_exec(): SQL error: %s in %s on line %d
