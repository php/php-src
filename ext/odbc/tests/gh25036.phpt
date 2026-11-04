--TEST--
GH-25036 (odbc_prepare() double free when SQLDescribeParam() fails)
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

$live = odbc_exec($conn, 'SELECT 1 AS a');

var_dump(odbc_prepare($conn, 'SELECT ? FROM gh25036_missing'));
var_dump(odbc_prepare($conn, 'SELECT ?, ? FROM gh25036_missing'));

var_dump(odbc_fetch_array($live));

odbc_close($conn);

try {
    odbc_fetch_row($live);
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
Warning: odbc_prepare(): SQL error: %s, SQL state %s in SQLDescribeParameter in %s on line %d
bool(false)

Warning: odbc_prepare(): SQL error: %s, SQL state %s in SQLDescribeParameter in %s on line %d
bool(false)
array(1) {
  ["a"]=>
  string(1) "1"
}
Error: ODBC result has already been closed
