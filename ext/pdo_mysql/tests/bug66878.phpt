--TEST--
Bug #66878: Multiple rowsets not returned unless PDO statement object is unset()
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

$pdo = MySQLPDOTest::factory();

$sql = 'SELECT 123; SELECT 42; SELECT 999';

$stmt = $pdo->query($sql);
var_dump($stmt->nextRowset());
var_dump($stmt->nextRowset());
var_dump($stmt->nextRowset());
$stmt->closeCursor();

$stmt = $pdo->query($sql);
var_dump($stmt->nextRowset());
var_dump($stmt->nextRowset());
var_dump($stmt->nextRowset());
$stmt->closeCursor();

?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
