--TEST--
Bug #36727 (segfault in bindValue() when no parameters are defined)
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
require __DIR__ . '/config.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
require __DIR__ . '/config.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');

$stmt = $db->prepare('SELECT * FROM test36727');
var_dump($stmt->bindValue(':test', 1, PDO::PARAM_INT));

echo "Done\n";
?>
--CLEAN--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->exec("DROP TABLE IF EXIST test36727");
?>
--EXPECTF--
Warning: PDOStatement::bindValue(): SQLSTATE[HY093]: Invalid parameter number: :test in %s on line %d
bool(false)
Done
