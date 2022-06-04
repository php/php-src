--TEST--
PDO_Firebird: rowCount
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

$dbh = getDbConnection();
$dbh->exec('CREATE TABLE test_rowcount (A VARCHAR(10))');
$dbh->exec("INSERT INTO test_rowcount VALUES ('A')");
$dbh->exec("INSERT INTO test_rowcount VALUES ('A')");
$dbh->exec("INSERT INTO test_rowcount VALUES ('B')");

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

$stmt = $dbh->prepare('DELETE FROM test_rowcount');
$stmt->execute();
var_dump($stmt->rowCount());

unset($stmt);
unset($dbh);

?>
--CLEAN--
<?php
require 'testdb.inc';
$dbh = getDbConnection();
@$dbh->exec('DROP TABLE test_rowcount');
unset($dbh);
--EXPECT--
bool(false)
int(2)
int(1)
int(3)
