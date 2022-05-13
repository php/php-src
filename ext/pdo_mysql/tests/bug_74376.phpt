--TEST--
Bug #74376 (Invalid free of persistent results on error/connection loss)
--EXTENSIONS--
pdo
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

$attr = getenv('PDOTEST_ATTR');
$attr = $attr ? unserialize($attr) : [];
$attr[PDO::ATTR_PERSISTENT] = true;
$attr[PDO::ATTR_EMULATE_PREPARES] = false;

putenv('PDOTEST_ATTR=' . serialize($attr));

$db = MySQLPDOTest::factory();
$stmt = $db->query("select (select 1 union select 2)");

print "ok";
?>
--EXPECTF--

Warning: PDO::query(): SQLSTATE[21000]: Cardinality violation: 1242 Subquery returns more than 1 row in %s on line %d
ok
