--TEST--
PDO_DBLIB: bigint columns are returned as strings
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require __DIR__ . '/config.inc';
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

// on 64-bit machines, these columns should come back as ints
// on 32-bit machines, they will come back as strings because zend_long isn't big enough
$expected = PHP_INT_SIZE == 8 ? 1 : '1';

$stmt = $db->query('SELECT CAST(1 AS bigint)');
var_dump($stmt->fetchColumn() === $expected);
?>
--EXPECT--
bool(true)
