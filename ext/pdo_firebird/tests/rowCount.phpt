--TEST--
PDO_Firebird: rowCount
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--ENV--
LSAN_OPTIONS=detect_leaks=0
--FILE--
<?php

require("testdb.inc");

$dbh->exec('CREATE TABLE test_rowcount (A VARCHAR(10))');
$dbh->exec("INSERT INTO test_rowcount VALUES ('A')");
$dbh->exec("INSERT INTO test_rowcount VALUES ('A')");
$dbh->exec("INSERT INTO test_rowcount VALUES ('B')");
$dbh->commit();

$query = "SELECT * FROM test_rowcount WHERE A = ?";

$stmt = $dbh->prepare($query);
$stmt->execute(array('A'));
$rows = $stmt->fetch();
$rows = $stmt->fetch();
var_dump($stmt->fetch());
var_dump($stmt->rowCount());

$stmt = $dbh->prepare('UPDATE test_rowcount SET A="A" WHERE A != ?');
$stmt->execute(array('A'));
var_dump($stmt->rowCount());
$dbh->commit();

$stmt = $dbh->prepare('DELETE FROM test_rowcount');
$stmt->execute();
var_dump($stmt->rowCount());

$dbh->commit();

unset($stmt);
unset($dbh);

?>
--CLEAN--
<?php
require 'testdb.inc';
@$dbh->exec('DROP TABLE test_rowcount');
--EXPECT--
bool(false)
int(2)
int(1)
int(3)
