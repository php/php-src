--TEST--
Bug #44618 (Fetching may rely on uninitialized data)
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
include __DIR__ . "/config.inc";
$conn = odbc_connect($dsn, $user, $pass, SQL_CUR_USE_ODBC);

odbc_exec($conn, "CREATE TABLE bug44618(ID INT, real1 REAL, text1 TEXT)");
odbc_exec($conn, "INSERT INTO bug44618 VALUES (1, 10.0199995, 'testing 1,2,3')");

$result = odbc_exec($conn, "SELECT * FROM bug44618");
var_dump(odbc_fetch_array($result));
$result = null;

$result = odbc_exec($conn, "SELECT * FROM bug44618");
odbc_fetch_into($result, $array);
var_dump($array);
$result = null;

$result = odbc_exec($conn, "SELECT * FROM bug44618");
odbc_fetch_row($result);
var_dump(odbc_result($result, "text1"));
$result = null;

$result = odbc_exec($conn, "SELECT * FROM bug44618");
odbc_result_all($result);
$result = null;
?>
--CLEAN--
<?php
include __DIR__ . "/config.inc";
$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, "DROP TABLE bug44618");
?>
--EXPECTF--
Warning: odbc_fetch_array(): Cannot get data of column #3 (retcode 100) in %s on line %d
array(3) {
  ["ID"]=>
  string(1) "1"
  ["real1"]=>
  string(5) "10.02"
  ["text1"]=>
  bool(false)
}

Warning: odbc_fetch_into(): Cannot get data of column #3 (retcode 100) in %s on line %d
array(3) {
  [0]=>
  string(1) "1"
  [1]=>
  string(5) "10.02"
  [2]=>
  bool(false)
}

Warning: odbc_result(): Cannot get data of column #3 (retcode 100) in %s on line %d
bool(false)
<table><tr><th>ID</th><th>real1</th><th>text1</th></tr>
<tr><td>1</td><td>10.02</td><td></td></tr></table>
Warning: odbc_result_all(): Cannot get data of column #3 (retcode 100) in %s on line %d
