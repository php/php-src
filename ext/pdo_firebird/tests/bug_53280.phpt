--TEST--
PDO_Firebird: bug 53280 segfaults if query column count is less than param count
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php

require("testdb.inc");

$dbh->exec('CREATE TABLE test53280(A VARCHAR(30), B VARCHAR(30), C VARCHAR(30))');
$dbh->exec("INSERT INTO test53280 VALUES ('A', 'B', 'C')");

$stmt1 = "SELECT B FROM test53280 WHERE A = ? AND B = ?";
$stmt2 = "SELECT B, C FROM test53280 WHERE A = ? AND B = ?";

$stmth2 = $dbh->prepare($stmt2);
$stmth2->execute(array('A', 'B'));
$rows = $stmth2->fetchAll(); // <------ OK
var_dump($rows);

$stmth1 = $dbh->prepare($stmt1);
$stmth1->execute(array('A', 'B'));
$rows = $stmth1->fetchAll(); // <------- segfault
var_dump($rows);

unset($stmth1);
unset($stmth2);
unset($stmt);
unset($dbh);

?>
--CLEAN--
<?php
require 'testdb.inc';
@$dbh->exec("DROP TABLE test53280");
unset($dbh);
?>
--EXPECT--
array(1) {
  [0]=>
  array(4) {
    ["B"]=>
    string(1) "B"
    [0]=>
    string(1) "B"
    ["C"]=>
    string(1) "C"
    [1]=>
    string(1) "C"
  }
}
array(1) {
  [0]=>
  array(2) {
    ["B"]=>
    string(1) "B"
    [0]=>
    string(1) "B"
  }
}
