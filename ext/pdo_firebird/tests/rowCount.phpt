--TEST--
PDO_Firebird: rowCount
--SKIPIF--
<?php require('skipif.inc'); ?>
--ENV--
LSAN_OPTIONS=detect_leaks=0
--FILE--
<?php

require("testdb.inc");

$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
@$dbh->exec('DROP TABLE testz');
$dbh->exec('CREATE TABLE testz (A VARCHAR(10))');
$dbh->exec("INSERT INTO testz VALUES ('A')");
$dbh->exec("INSERT INTO testz VALUES ('A')");
$dbh->exec("INSERT INTO testz VALUES ('B')");
$dbh->commit();

$query = "SELECT * FROM testz WHERE A = ?";

$stmt = $dbh->prepare($query);
$stmt->execute(array('A'));
$rows = $stmt->fetch();
$rows = $stmt->fetch();
var_dump($stmt->fetch());
var_dump($stmt->rowCount());

$stmt = $dbh->prepare('UPDATE testZ SET A="A" WHERE A != ?');
$stmt->execute(array('A'));
var_dump($stmt->rowCount());
$dbh->commit();

$stmt = $dbh->prepare('DELETE FROM testz');
$stmt->execute();
var_dump($stmt->rowCount());

$dbh->commit();

$dbh->exec('DROP TABLE testz');

unset($stmt);
unset($dbh);

?>
--EXPECT--
bool(false)
int(2)
int(1)
int(3)
