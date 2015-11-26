--TEST--
SQLite3Stmt::clear test with parameters
--CREDITS--
Thijs Feryn <thijs@feryn.eu>
#TestFest PHPBelgium 2009
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php
$db = new SQLite3(':memory:');
define('TIMENOW', time());
echo "Creating Table\n";
$db->exec('CREATE TABLE test (time INTEGER, id STRING)');
echo "INSERT into table\n";
var_dump($db->exec("INSERT INTO test (time, id) VALUES (" . TIMENOW . ", 'b')"));

echo "SELECTING results\n";
$stmt = $db->prepare("SELECT * FROM test WHERE id = ? ORDER BY id ASC");
var_dump($stmt->clear('invalid argument'));
echo "Closing database\n";
var_dump($db->close());
echo "Done\n";
?>
--EXPECTF--
Creating Table
INSERT into table
bool(true)
SELECTING results

Warning: SQLite3Stmt::clear() expects exactly 0 parameters, 1 given in %s on line %d
NULL
Closing database
bool(true)
Done
