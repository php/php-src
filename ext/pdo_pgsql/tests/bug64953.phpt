--TEST--
PDO PgSQL Bug #64953 (Postgres prepared statement positional parameter casting)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
echo "Test\n";

require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$pdo = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$pdo->setAttribute (\PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION);

echo "Taken from the bug report:\n";

$st = $pdo->prepare('SELECT ?::char as i');
$st->bindValue(1, '1');
$st->execute();
var_dump($st->fetch()); // return false


$st = $pdo->prepare('SELECT (?)::char as i');
$st->bindValue(1, '1');
$st->execute();
var_dump($st->fetch());  // return array(1) { ["i"]=> string(1) "1" }

echo "Something more nasty:\n";

$st = $pdo->prepare("SELECT :int::int as i");
$st->execute(array(":int" => 123));
var_dump($st->fetch());

$st = $pdo->prepare("SELECT '''?'''::text as \":text\"");
$st->execute();
var_dump($st->fetch());

?>
Done
--EXPECT--
Test
Taken from the bug report:
array(2) {
  ["i"]=>
  string(1) "1"
  [0]=>
  string(1) "1"
}
array(2) {
  ["i"]=>
  string(1) "1"
  [0]=>
  string(1) "1"
}
Something more nasty:
array(2) {
  ["i"]=>
  string(3) "123"
  [0]=>
  string(3) "123"
}
array(2) {
  [":text"]=>
  string(3) "'?'"
  [0]=>
  string(3) "'?'"
}
Done
