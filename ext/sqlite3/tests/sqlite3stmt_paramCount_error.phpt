--TEST--
SQLite3Stmt::paramCount error test
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

require_once(dirname(__FILE__) . '/new_db.inc');

echo "Creating Table\n";
var_dump($db->exec('CREATE TABLE foobar (id INTEGER, name STRING, city STRING)'));

echo "INSERT into table\n";
var_dump($db->exec("INSERT INTO foobar (id, name, city) VALUES (1, 'john', 'LA')"));
var_dump($db->exec("INSERT INTO foobar (id, name, city) VALUES (2, 'doe', 'SF')"));


$query = "SELECT * FROM foobar WHERE id = ? ORDER BY id ASC";

echo "SELECTING results\n";

$stmt = $db->prepare($query);

echo "paramCount with wrong number of arguments\n";
var_dump($stmt->paramCount('foobar'));
$result = $stmt->execute();
echo "Closing database\n";
$stmt = null;
$result = null;
var_dump($db->close());
echo "Done\n";
?>
--EXPECTF--
Creating Table
bool(true)
INSERT into table
bool(true)
bool(true)
SELECTING results
paramCount with wrong number of arguments

Warning: SQLite3Stmt::paramCount() expects exactly 0 parameters, 1 given in %s on line %d
NULL
Closing database
bool(true)
Done
