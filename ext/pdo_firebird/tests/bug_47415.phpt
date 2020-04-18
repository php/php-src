--TEST--
Bug #47415 PDO_Firebird segfaults when passing lowercased column name to bindColumn()
--SKIPIF--
<?php require('skipif.inc'); ?>
--FILE--
<?php
require 'testdb.inc';

@$dbh->exec('DROP TABLE testz');
$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);

$dbh->exec('CREATE TABLE testz (idx int NOT NULL PRIMARY KEY, txt VARCHAR(20))');
$dbh->exec('INSERT INTO testz VALUES(0, \'String0\')');

$dbh->commit();

$query = "SELECT idx, txt FROM testz ORDER by idx";
$idx = $txt = 0;
$stmt = $dbh->prepare($query);
$stmt->bindColumn('idx', $idx);
$stmt->bindColumn('txt', $txt);
$stmt->execute();
$rows = $stmt->fetch(PDO::FETCH_BOUND);
var_dump($stmt->fetch());
var_dump($stmt->rowCount());


$stmt = $dbh->prepare('DELETE FROM testz');
$stmt->execute();

$dbh->commit();

unset($stmt);
unset($dbh);
?>
--EXPECT--
bool(false)
int(1)
