--TEST--
odbc_commit(): Basic test for odbc_commit()
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'CREATE TABLE commit_table (test INT)');

odbc_autocommit($conn, false);
odbc_exec($conn, 'INSERT INTO commit_table VALUES(1)');
var_dump(odbc_commit($conn));

$res = odbc_exec($conn, 'SELECT * FROM commit_table');
var_dump(odbc_result($res, "test"));

?>
--CLEAN--
<?php
require 'config.inc';
$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'DROP TABLE commit_table');
?>
--EXPECT--
bool(true)
string(1) "1"
