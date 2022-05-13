--TEST--
odbc_exec(): Basic test
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
<?php
    if ("unixODBC" != ODBC_TYPE) {
        die("skip ODBC_TYPE != unixODBC");
    }
?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'foo', 'bar');
odbc_exec($conn, 'foo');

odbc_exec($conn, '', '');
odbc_exec($conn, '');

odbc_exec($conn, 1, 1);
odbc_exec($conn, 1);

odbc_exec($conn, NULL, NULL);
odbc_exec($conn, NULL);

?>
--EXPECTF--
Warning: odbc_exec(): Argument #3 must be of type int, string given in %s on line %d

Warning: odbc_exec(): SQL error: %s in %s on line %d

Warning: odbc_exec(): Argument #3 must be of type int, string given in %s on line %d

Warning: odbc_exec(): SQL error: %s in %s on line %d

Warning: odbc_exec(): SQL error: %s in %s on line %d

Warning: odbc_exec(): SQL error: %s in %s on line %d

Warning: odbc_exec(): SQL error: %s in %s on line %d

Warning: odbc_exec(): SQL error: %s in %s on line %d
