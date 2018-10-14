--TEST--
Bug #51670 (getColumnMeta causes segfault when re-executing query after calling nextRowset)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$query = $db->prepare('SELECT 1 AS num');
$query->execute();
if(!is_array($query->getColumnMeta(0))) die('FAIL!');
$query->nextRowset();
$query->execute();
if(!is_array($query->getColumnMeta(0))) die('FAIL!');
echo 'done!';
?>
--EXPECTF--
done!
