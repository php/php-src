--TEST--
Bug #46274 (pdo_pgsql - Segfault when using PDO::ATTR_STRINGIFY_FETCHES and blob)
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

$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

try {
    @$db->query("SET bytea_output = 'escape'");
} catch (Exception $e) {
}

$db->query('CREATE TABLE test_one_blob (id SERIAL NOT NULL, blob1 BYTEA)');

$stmt = $db->prepare("INSERT INTO test_one_blob (blob1) VALUES (:foo)");

$data = 'foo';
$blob = fopen('php://memory', 'a');
fwrite($blob, $data);
rewind($blob);

$stmt->bindparam(':foo', $blob, PDO::PARAM_LOB);
$stmt->execute();

$blob = '';
$stmt->bindparam(':foo', $blob, PDO::PARAM_LOB);
$stmt->execute();

$data = '';
$blob = fopen('php://memory', 'a');
fwrite($blob, $data);
rewind($blob);

$stmt->bindparam(':foo', $blob, PDO::PARAM_LOB);
$stmt->execute();

$blob = NULL;
$stmt->bindparam(':foo', $blob, PDO::PARAM_LOB);
$stmt->execute();

$res = $db->query("SELECT blob1 from test_one_blob");
// Resource
var_dump($res->fetch());

// Empty string
var_dump($res->fetch());

// Empty string
var_dump($res->fetch());

// NULL
var_dump($res->fetch());

$db->query('DROP TABLE test_one_blob');

?>
--EXPECT--
array(2) {
  ["blob1"]=>
  string(3) "foo"
  [0]=>
  string(3) "foo"
}
array(2) {
  ["blob1"]=>
  string(0) ""
  [0]=>
  string(0) ""
}
array(2) {
  ["blob1"]=>
  string(0) ""
  [0]=>
  string(0) ""
}
array(2) {
  ["blob1"]=>
  NULL
  [0]=>
  NULL
}
