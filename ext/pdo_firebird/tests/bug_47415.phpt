--TEST--
Bug #47415 PDO_Firebird segfaults when passing lowercased column name to bindColumn()
--SKIPIF--
<?php extension_loaded("pdo_firebird") or die("skip"); ?>
<?php function_exists("ibase_query") or die("skip"); ?>
--FILE--
<?php

require("testdb.inc");

$dbh = new PDO("firebird:dbname=$test_base",$user,$password) or die;
$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
$value = '2';
@$dbh->exec('DROP TABLE testz');
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

$dbh->exec('DROP TABLE testz');

unset($stmt);
unset($dbh);

?>
--EXPECT--
bool(false)
int(1)
