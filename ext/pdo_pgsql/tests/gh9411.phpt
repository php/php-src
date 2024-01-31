--TEST--
Bug GH-9411 (PgSQL large object resource is incorrectly closed)
--EXTENSIONS--
pdo
pdo_pgsql
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
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

$db->beginTransaction();
$oid = $db->pgsqlLOBCreate();
var_dump($lob = $db->pgsqlLOBOpen($oid, 'wb'));
fwrite($lob, 'test');
$db->rollback();
var_dump($lob);

$db->beginTransaction();
$oid = $db->pgsqlLOBCreate();
var_dump($lob = $db->pgsqlLOBOpen($oid, 'wb'));
fwrite($lob, 'test');
$db->commit();
var_dump($lob);

$db->beginTransaction();
var_dump($lob = $db->pgsqlLOBOpen($oid, 'wb'));
var_dump(fgets($lob));
?>
--EXPECTF--
resource(%d) of type (stream)
resource(%d) of type (Unknown)
resource(%d) of type (stream)
resource(%d) of type (Unknown)
resource(%d) of type (stream)
string(4) "test"
