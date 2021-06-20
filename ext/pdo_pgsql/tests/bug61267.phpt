--TEST--
PDO::exec() returns 0 when the statement is a SELECT.
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

$res = $db->exec('SELECT * from generate_series(1, 42);');
var_dump($res);
echo "Done\n";
?>
--EXPECT--
int(0)
Done
