--TEST--
PDO::exec() returns 0 when the statement is a SELECT.
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
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
