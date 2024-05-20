--TEST--
odbc_free_result(): Basic test
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'CREATE TABLE free_result (TEST INT NOT NULL)');

odbc_exec($conn, 'INSERT INTO free_result VALUES (1), (2)');

$res = odbc_exec($conn, 'SELECT * FROM free_result');

try {
    var_dump(odbc_fetch_row($res));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(odbc_result($res, 'test'));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(odbc_free_result($res));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(odbc_free_result($conn));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(odbc_fetch_row($res));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(odbc_result($res, 'test'));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--CLEAN--
<?php
require 'config.inc';
$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'DROP TABLE free_result');
?>
--EXPECT--
bool(true)
string(1) "1"
bool(true)
odbc_free_result(): Argument #1 ($statement) must be of type Odbc\Result, Odbc\Connection given
ODBC result has already been closed
ODBC result has already been closed
