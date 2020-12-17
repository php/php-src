--TEST--
Bug #36727 (segfault in bindValue() when no parameters are defined)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
require __DIR__ . '/config.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
require __DIR__ . '/config.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');

$stmt = $db->prepare('SELECT * FROM child');
var_dump($stmt->bindValue(':test', 1, PDO::PARAM_INT));

echo "Done\n";
?>
--EXPECTF--
Warning: PDOStatement::bindValue(): SQLSTATE[HY093]: Invalid parameter number: :test in %s on line %d
bool(false)
Done
