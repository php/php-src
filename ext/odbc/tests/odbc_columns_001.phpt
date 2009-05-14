--TEST--
odbc_columns(): Basic test
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

var_dump($result = odbc_columns($conn, '', '', '', ''));
var_dump(odbc_fetch_row($result));

var_dump($result = odbc_columns($conn, NULL, NULL, NULL, NULL));
var_dump(odbc_fetch_row($result));

var_dump($result = odbc_columns($conn, 'FOO', 'FOO', 'FOO', 'FOO'));
var_dump(odbc_fetch_row($result));

?>
--EXPECTF--
resource(%d) of type (odbc result)
bool(false)
resource(%d) of type (odbc result)
bool(false)

Warning: odbc_columns(): SQL error: Failed to fetch error message, SQL state HY000 in SQLColumns in %s on line %d
bool(false)

Warning: odbc_fetch_row() expects parameter 1 to be resource, boolean given in %s on line %d
NULL
