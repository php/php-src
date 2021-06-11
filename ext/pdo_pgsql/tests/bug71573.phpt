--TEST--
Bug #71573 (Segfault (core dumped) if paramno beyond bound)
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require_once __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
require_once __DIR__ . '/config.inc';
PDOTest::skip();
?>
--FILE--
<?php
require_once __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
require_once __DIR__ . '/config.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');

$statement = $db->prepare('select ?');
$statement->execute([ 'test', 'test', 'test' ]);

?>
--EXPECTF--
Warning: PDOStatement::execute(): SQLSTATE[HY093]: Invalid parameter number: parameter was not defined in %s on line %d
