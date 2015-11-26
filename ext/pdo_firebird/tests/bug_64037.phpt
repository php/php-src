--TEST--
Bug #64037 Firebird return wrong value for numeric field
--SKIPIF--
<?php extension_loaded("pdo_firebird") or die("skip"); ?>
<?php function_exists("ibase_query") or die("skip"); ?>
--FILE--
<?php

require("testdb.inc");

$dbh = new PDO("firebird:dbname=$test_base",$user,$password) or die;
$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
$value = '2';
@$dbh->exec('DROP TABLE price');
$dbh->exec("CREATE TABLE PRICE (ID INTEGER NOT NULL, TEXT VARCHAR(10), COST NUMERIC(15, 2))");
$dbh->exec("INSERT INTO PRICE (ID, TEXT, COST) VALUES (1, 'test', -1.0)");
$dbh->exec("INSERT INTO PRICE (ID, TEXT, COST) VALUES (2, 'test', -0.99)");
$dbh->exec("INSERT INTO PRICE (ID, TEXT, COST) VALUES (3, 'test', -1.01)");

$dbh->commit();

$query = "SELECT * from price order by ID";
$stmt = $dbh->prepare($query);
$stmt->execute();
$rows = $stmt->fetchAll();
var_dump($rows[0]['COST']);
var_dump($rows[1]['COST']);
var_dump($rows[2]['COST']);


$stmt = $dbh->prepare('DELETE FROM price');
$stmt->execute();

$dbh->commit();

$dbh->exec('DROP TABLE price');

unset($stmt);
unset($dbh);

?>
--EXPECT--
string(5) "-1.00"
string(5) "-0.99"
string(5) "-1.01"