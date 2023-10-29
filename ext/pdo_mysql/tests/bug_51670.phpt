--TEST--
Bug #51670 (getColumnMeta causes segfault when re-executing query after calling nextRowset)
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
$db = MySQLPDOTest::factory();
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
