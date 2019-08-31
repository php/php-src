--TEST--
PDO PgSQL PGSQL_ATTR_DISABLE_PREPARES
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_COLUMN);

$stmt = $db->prepare("SELECT statement FROM pg_prepared_statements", array(
	PDO::ATTR_EMULATE_PREPARES => true));

$stmt2 = $db->prepare("SELECT (?)::int2");
$stmt2->execute(array(1));
var_dump($stmt2->fetch());
$stmt2->execute(array(2));
var_dump($stmt2->fetch());

$stmt->execute();
$first = $stmt->fetchAll();

$stmt3 = $db->prepare("SELECT (?)::int4", array(
	PDO::PGSQL_ATTR_DISABLE_PREPARES => true));
$stmt3->execute(array(3));
var_dump($stmt3->fetch());
$stmt3->execute(array(4));
var_dump($stmt3->fetch());

$stmt->execute();
$second = $stmt->fetchAll();

var_dump($first, $second);

?>
--EXPECT--
string(1) "1"
string(1) "2"
string(1) "3"
string(1) "4"
array(1) {
  [0]=>
  string(17) "SELECT ($1)::int2"
}
array(1) {
  [0]=>
  string(17) "SELECT ($1)::int2"
}
