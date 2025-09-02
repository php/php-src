--TEST--
odbc_tables(): Basic test
--EXTENSIONS--
odbc
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
object(Odbc\Result)#%d (%d) {
}
bool(false)
object(Odbc\Result)#%d (%d) {
}
bool(true)
bool(true)
object(Odbc\Result)#%d (%d) {
}
bool(true)
bool(true)
object(Odbc\Result)#%d (%d) {
}
bool(false)
