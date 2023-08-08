--TEST--
odbc_num_fields(): Getting the number of fields
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'CREATE TABLE num_fields (foo INT, bar INT, baz INT)');
odbc_exec($conn, 'INSERT INTO num_fields VALUES (1, 2, 3)');

$res = odbc_exec($conn, 'SELECT * FROM num_fields');
var_dump(odbc_num_fields($res));
odbc_free_result($res);

$res = odbc_exec($conn, 'SELECT * FROM num_fields WHERE foo = 4');
var_dump(odbc_num_fields($res));
odbc_free_result($res);

odbc_close($conn);
?>
--CLEAN--
<?php
require 'config.inc';
$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'DROP TABLE num_fields');
?>
--EXPECT--
int(3)
int(3)
