--TEST--
odbc_tables(): Basic test
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

var_dump($result = odbc_tables($conn, '', '', '', ''));
var_dump(odbc_fetch_row($result));

var_dump($result = odbc_tables($conn));
var_dump(odbc_fetch_row($result));
var_dump(odbc_free_result($result));

var_dump($result = odbc_tables($conn, NULL, NULL, NULL, NULL));
var_dump(odbc_fetch_row($result));
var_dump(odbc_free_result($result));

var_dump($result = odbc_tables($conn, 'FOO', 'FOO', 'FOO', 'FOO'));
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
