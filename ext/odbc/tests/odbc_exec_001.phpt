--TEST--
odbc_exec(): Basic test
--SKIPIF--
<?php include 'skipif.inc'; ?>
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
Warning: odbc_exec() expects parameter 3 to be integer, %unicode_string_optional% given in %s on line %d

Warning: odbc_exec(): SQL error: %s in %s on line %d

Warning: odbc_exec() expects parameter 3 to be integer, %unicode_string_optional% given in %s on line %d

Warning: odbc_exec(): SQL error: %s in %s on line %d

Warning: odbc_exec(): SQL error: %s in %s on line %d

Warning: odbc_exec(): SQL error: %s in %s on line %d

Warning: odbc_exec(): SQL error: %s in %s on line %d

Warning: odbc_exec(): SQL error: %s in %s on line %d
