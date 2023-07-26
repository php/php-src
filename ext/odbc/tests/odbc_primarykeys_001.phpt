--TEST--
odbc_primarykeys(): Basic test for odbc_primarykeys()
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--XFAIL--
Doesn't work with MS SQL
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'CREATE DATABASE PrimarykeysTest');
odbc_exec($conn, 'CREATE TABLE primarykeys (test INT NOT NULL)');

$res = odbc_primarykeys($conn, "", "PrimarykeysTest", "primarykeys");
var_dump(odbc_fetch_array($res));
odbc_free_result($res);

odbc_exec($conn, 'ALTER TABLE primarykeys ADD CONSTRAINT primarykeys_pk PRIMARY KEY (test)');

$res = odbc_primarykeys($conn, "", "PrimarykeysTest", "primarykeys");
var_dump(odbc_fetch_array($res));
odbc_free_result($res);

?>
--CLEAN--
<?php
require 'config.inc';
$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'DROP TABLE primarykeys');
odbc_exec($conn, 'DROP DATABASE PrimarykeysTest');
?>
--EXPECT--
bool(false)
