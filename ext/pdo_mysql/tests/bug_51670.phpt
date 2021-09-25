--TEST--
Bug #51670 (getColumnMeta causes segfault when re-executing query after calling nextRowset)
--EXTENSIONS--
pdo
pdo_mysql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$query = $db->prepare('SELECT 1 AS num');
$query->execute();
if(!is_array($query->getColumnMeta(0))) die('FAIL!');
$query->nextRowset();
$query->execute();
if(!is_array($query->getColumnMeta(0))) die('FAIL!');
echo 'done!';
?>
--EXPECT--
done!
