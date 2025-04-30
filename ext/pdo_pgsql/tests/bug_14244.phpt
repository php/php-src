--TEST--
PDO PgSQL Bug #14244 (Postgres sees parameters in a dollar-delimited string)
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
echo "Test\n";

require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt');
$pdo->setAttribute (\PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION);
$pdo->setAttribute (\PDO::ATTR_DEFAULT_FETCH_MODE, \PDO::FETCH_ASSOC);

echo "Already working (see bug64953.phpt):\n";

$st = $pdo->prepare("SELECT '?' question");
$st->execute();
var_dump($st->fetch());

echo "Inside a dollar-quoted string:\n";

$st = $pdo->prepare("SELECT \$\$?\$\$ question");
$st->execute();
var_dump($st->fetch());

?>
Done
--EXPECT--
Test
Already working (see bug64953.phpt):
array(1) {
  ["question"]=>
  string(1) "?"
}
Inside a dollar-quoted string:
array(1) {
  ["question"]=>
  string(1) "?"
}
Done
