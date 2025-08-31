--TEST--
Bug #71171 odbc_fetch_array generates SIGFAULT, variant 0
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'CREATE TABLE bug71171 (ID INT, VARCHAR_COL NVARCHAR(40))');

odbc_exec($conn, "INSERT INTO bug71171(ID, VARCHAR_COL) VALUES (1, '" . chr(0x81) . "')");

$res = odbc_exec($conn,"SELECT ID FROM bug71171 WHERE VARCHAR_COL = '" . chr(0x81) . "'");
if ($res) {
    while($record = odbc_fetch_array($res)) var_dump($record);
}

odbc_close($conn);
?>
--CLEAN--
<?php
include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'DROP TABLE bug71171');

odbc_close($conn);

?>
--EXPECT--
array(1) {
  ["ID"]=>
  string(1) "1"
}
