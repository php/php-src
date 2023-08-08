--TEST--
odbc_primarykeys(): Basic test for odbc_primarykeys()
--EXTENSIONS--
odbc
--SKIPIF--
<?php
include 'skipif.inc';

$conn = odbc_connect($dsn, $user, $pass);
$result = @odbc_exec($conn, "SELECT @@Version");
if ($result) {
    $array = odbc_fetch_array($result);
    $info = (string) reset($array);
    if (!str_contains($info, "Microsoft SQL Server")) {
       echo "skip MS SQL specific test";
    }
}

?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'CREATE DATABASE PrimarykeysTest');
odbc_exec($conn, 'USE PrimarykeysTest');
odbc_exec($conn, 'CREATE TABLE primarykeys (test INT NOT NULL)');

$res = odbc_primarykeys($conn, "PrimarykeysTest", "dbo", "primarykeys");
var_dump(odbc_fetch_array($res));
odbc_free_result($res);

odbc_exec($conn, 'ALTER TABLE primarykeys ADD CONSTRAINT primarykeys_pk PRIMARY KEY (test)');

$res = odbc_primarykeys($conn, "PrimarykeysTest", "dbo", "");
var_dump(odbc_fetch_array($res));
odbc_free_result($res);

$res = odbc_primarykeys($conn, "PrimarykeysTest", "", "primarykeys");
var_dump(odbc_fetch_array($res));
odbc_free_result($res);

$res = odbc_primarykeys($conn, null, "dbo", "primarykeys");
var_dump(odbc_fetch_array($res));
odbc_free_result($res);

$res = odbc_primarykeys($conn, "PrimarykeysTest", "dbo", "primarykeys");
var_dump(odbc_fetch_array($res));
odbc_free_result($res);

?>
--CLEAN--
<?php
require 'config.inc';
$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'USE PrimarykeysTest');
odbc_exec($conn, 'DROP TABLE primarykeys');
odbc_exec($conn, 'USE master');
odbc_exec($conn, 'DROP DATABASE PrimarykeysTest');
?>
--EXPECTF--
bool(false)
bool(false)
bool(false)
array(%d) {
  ["TABLE_CAT"]=>
  string(%d) "PrimarykeysTest"
  ["TABLE_SCHEM"]=>
  string(%d) "dbo"
  ["TABLE_NAME"]=>
  string(%d) "primarykeys"
  ["COLUMN_NAME"]=>
  string(%d) "test"
  ["KEY_SEQ"]=>
  string(%d) "1"
  ["PK_NAME"]=>
  string(%d) "primarykeys_pk"
}
array(%d) {
  ["TABLE_CAT"]=>
  string(%d) "PrimarykeysTest"
  ["TABLE_SCHEM"]=>
  string(%d) "dbo"
  ["TABLE_NAME"]=>
  string(%d) "primarykeys"
  ["COLUMN_NAME"]=>
  string(%d) "test"
  ["KEY_SEQ"]=>
  string(%d) "1"
  ["PK_NAME"]=>
  string(%d) "primarykeys_pk"
}
