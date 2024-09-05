--TEST--
PDO PgSQL PDOStatement::debugDumpParams() with emulated prepares
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
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);

$stmt = $db->prepare('SELECT :bool, :int, :string, :null');
$stmt->bindValue(':bool', true, PDO::PARAM_BOOL);
$stmt->bindValue(':int', 123, PDO::PARAM_INT);
$stmt->bindValue(':string', 'foo', PDO::PARAM_STR);
$stmt->bindValue(':null', null, PDO::PARAM_NULL);
$stmt->execute();

var_dump($stmt->debugDumpParams());

?>
--EXPECT--
SQL: [34] SELECT :bool, :int, :string, :null
Sent SQL: [28] SELECT 't', 123, 'foo', NULL
Params:  4
Key: Name: [5] :bool
paramno=-1
name=[5] ":bool"
is_param=1
param_type=2
Key: Name: [4] :int
paramno=-1
name=[4] ":int"
is_param=1
param_type=1
Key: Name: [7] :string
paramno=-1
name=[7] ":string"
is_param=1
param_type=2
Key: Name: [5] :null
paramno=-1
name=[5] ":null"
is_param=1
param_type=0
NULL
