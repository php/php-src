--TEST--
Bug #80152 (odbc_execute() moves internal pointer of $params)
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn,"CREATE TABLE bug80152 (id INT, name CHAR(24))"); 
$stmt = odbc_prepare($conn,"INSERT INTO bug80152 (id, name) VALUES (?, ?)");
$params = [1, "John", "Lim"];
var_dump(key($params));
odbc_execute($stmt, $params);
var_dump(key($params));
?>
--CLEAN--
<?php
include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, "DROP TABLE bug80152");
?>
--EXPECT--
int(0)
int(0)
