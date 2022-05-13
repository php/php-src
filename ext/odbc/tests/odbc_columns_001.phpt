--TEST--
odbc_columns(): Basic test
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

var_dump($result = odbc_columns($conn, '', '', '', ''));
var_dump(odbc_fetch_row($result));

var_dump($result = odbc_columns($conn));
var_dump(odbc_fetch_row($result));
var_dump(odbc_free_result($result));

var_dump($result = odbc_columns($conn, NULL, NULL, NULL, NULL));
var_dump(odbc_fetch_row($result));
var_dump(odbc_free_result($result));

var_dump($result = odbc_columns($conn, 'FOO', 'FOO', 'FOO', 'FOO'));
var_dump(odbc_fetch_row($result));

?>
--EXPECTF--
resource(%d) of type (odbc result)
bool(false)
resource(%d) of type (odbc result)
bool(true)
bool(true)
resource(%d) of type (odbc result)
bool(true)
bool(true)
resource(%d) of type (odbc result)
bool(false)
