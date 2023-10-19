--TEST--
Bug #47415 PDO_Firebird segfaults when passing lowercased column name to bindColumn()
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--ENV--
LSAN_OPTIONS=detect_leaks=0
--FILE--
<?php
require 'testdb.inc';

$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);

$dbh->exec('CREATE TABLE test47415 (idx int NOT NULL PRIMARY KEY, txt VARCHAR(20))');
$dbh->exec('INSERT INTO test47415 VALUES(0, \'String0\')');

$dbh->commit();

$query = "SELECT idx, txt FROM test47415 ORDER by idx";
$idx = $txt = 0;
$stmt = $dbh->prepare($query);
$stmt->bindColumn('idx', $idx);
$stmt->bindColumn('txt', $txt);
$stmt->execute();
$rows = $stmt->fetch(PDO::FETCH_BOUND);
var_dump($stmt->fetch());
var_dump($stmt->rowCount());


$stmt = $dbh->prepare('DELETE FROM test47415');
$stmt->execute();

$dbh->commit();

unset($stmt);
unset($dbh);
?>
--CLEAN--
<?php
require 'testdb.inc';
@$dbh->exec("DROP TABLE test47415");
?>
--EXPECT--
bool(false)
int(1)
