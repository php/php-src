--TEST--
PDO_Firebird: bug 48877 The "bindValue" and "bindParam" do not work for PDO Firebird if we use named parameters (:parameter).
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

$value = '2';

$dbh = getDbConnection();
$dbh->exec('CREATE TABLE test48877 (A integer)');
$dbh->exec("INSERT INTO test48877 VALUES ('1')");
$dbh->exec("INSERT INTO test48877 VALUES ('2')");
$dbh->exec("INSERT INTO test48877 VALUES ('3')");

$query = "SELECT * FROM test48877 WHERE A = :paramno";

$stmt = $dbh->prepare($query);
$stmt->bindParam(':paramno', $value, PDO::PARAM_STR);
$stmt->execute();
$rows = $stmt->fetch();
var_dump($stmt->fetch());
var_dump($stmt->rowCount());


$stmt = $dbh->prepare('DELETE FROM test48877');
$stmt->execute();

unset($stmt);
unset($dbh);

?>
--CLEAN--
<?php
require 'testdb.inc';
$dbh = getDbConnection();
@$dbh->exec("DROP TABLE test48877");
unset($dbh);
?>
--EXPECT--
bool(false)
int(1)
