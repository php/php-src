--TEST--
PDO_DBLIB: bigint columns are returned as strings
--EXTENSIONS--
pdo_dblib
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
$db = getDbConnection();
if (in_array($db->getAttribute(PDO::DBLIB_ATTR_TDS_VERSION), ['4.2', '4.6', '5.0', '6.0', '7.0'])) die('skip bigint type is unsupported by active TDS version');
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

$db = getDbConnection();

// on 64-bit machines, these columns should come back as ints
// on 32-bit machines, they will come back as strings because zend_long isn't big enough
$expected = PHP_INT_SIZE == 8 ? 1 : '1';

$stmt = $db->query('SELECT CAST(1 AS bigint)');
var_dump($stmt->fetchColumn() === $expected);
?>
--EXPECT--
bool(true)
