--TEST--
PDO Common: PDORow + get_parent_class()
--SKIPIF--
<?php
if (!extension_loaded('pdo_sqlite')) die ("skip Need PDO_SQlite support");
?>
--FILE--
<?php
$db = new PDO('sqlite::memory:');
$db->exec('CREATE TABLE test (id int)');
$db->exec('INSERT INTO test VALUES (23)');

$stmt = $db->prepare('SELECT id FROM test');
$stmt->execute();
$result = $stmt->fetch(PDO::FETCH_LAZY);

echo get_class($result), "\n";
var_dump(get_parent_class($result));
?>
--EXPECT--
PDORow
bool(false)
