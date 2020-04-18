--TEST--
Bug #74376 (Invalid free of persistent results on error/connection loss)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
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
--EXPECT--
ok
