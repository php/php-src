--TEST--
odbc_rollback(): Basic test for odbc_rollback()
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'CREATE TABLE rollback_table (test INT)');

odbc_autocommit($conn, false);
odbc_exec($conn, 'INSERT INTO rollback_table VALUES(1)');
var_dump(odbc_rollback($conn));

$res = odbc_exec($conn, 'SELECT * FROM rollback_table');
var_dump(odbc_result($res, "test"));

?>
--CLEAN--
<?php
require 'config.inc';
$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'DROP TABLE rollback_table');
?>
--EXPECT--
bool(true)
bool(false)
