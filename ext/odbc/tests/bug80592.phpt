--TEST--
Bug #80592 (all floats are the same in ODBC parameters)
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn,"CREATE TABLE bug80592 (f1 float not null, f2 float not null, f3 float not null)"); 
$stmt = odbc_prepare($conn, "INSERT INTO bug80592 (f1, f2, f3) values (?, ?, ?)");
odbc_execute($stmt, [1.0, 2.0, 3.0]);
$res = odbc_exec($conn, "SELECT f1, f2, f3 from bug80592");
var_dump(odbc_fetch_array($res));
?>
--CLEAN--
<?php
include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, "DROP TABLE bug80592");
?>
--EXPECT--
array(3) {
  ["f1"]=>
  string(3) "1.0"
  ["f2"]=>
  string(3) "2.0"
  ["f3"]=>
  string(3) "3.0"
}
