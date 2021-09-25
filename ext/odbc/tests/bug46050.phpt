--TEST--
Bug #46050 (odbc_next_result corrupts prepared resource)
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
include __DIR__ . "/config.inc";
$conn = odbc_connect($dsn, $user, $pass);
$stmt = odbc_prepare($conn, "SELECT 1");
var_dump(odbc_execute($stmt));
var_dump(odbc_fetch_array($stmt));
var_dump(odbc_next_result($stmt));
var_dump(odbc_execute($stmt));
var_dump(odbc_fetch_array($stmt));
?>
--EXPECT--
bool(true)
array(1) {
  ["1"]=>
  string(1) "1"
}
bool(false)
bool(true)
array(1) {
  ["1"]=>
  string(1) "1"
}
