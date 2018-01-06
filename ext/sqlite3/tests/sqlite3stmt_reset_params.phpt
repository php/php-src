--TEST--
SQLite3Stmt::reset with parameter test
--CREDITS--
Jelle Lampaert
#Belgian Testfest 2009
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php
$db = new SQLite3(':memory:');
echo "Creating Table\n";
var_dump($db->exec('CREATE TABLE foobar (id INTEGER, name STRING)'));

echo "INSERT into table\n";
var_dump($db->exec("INSERT INTO foobar (id, name) VALUES (1, 'john')"));


$query = "SELECT name FROM foobar WHERE id = 1";

echo "Prepare query\n";
$stmt = $db->prepare($query);

echo "Reset query\n";
try {
  $stmt->reset("foo");
} catch (Exception $ex) {
  var_dump($ex->getMessage());
}

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
Prepare query
Reset query

Warning: SQLite3Stmt::reset() expects exactly 0 parameters, %d given in %s on line %d
Closing database
bool(true)
Done
