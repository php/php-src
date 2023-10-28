--TEST--
Bug #46274 (pdo_pgsql - Segfault when using PDO::ATTR_STRINGIFY_FETCHES and blob)
--EXTENSIONS--
pdo
pdo_oci
--SKIPIF--
<?php
require __DIR__.'/../../pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory('ext/pdo_oci/tests/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

$db->beginTransaction();

$db->query('CREATE TABLE test46274 (id INT NOT NULL, blob1 BLOB)');

$stmt = $db->prepare("INSERT INTO test46274 (id, blob1) VALUES (:id, EMPTY_BLOB()) RETURNING blob1 INTO :foo");

$data = 'foo';
$blob = fopen('php://memory', 'a');
fwrite($blob, $data);
rewind($blob);

$id = 1;
$stmt->bindparam(':id', $id);
$stmt->bindparam(':foo', $blob, PDO::PARAM_LOB);
$stmt->execute();

$data = '';
$blob = fopen('php://memory', 'a');
fwrite($blob, $data);
rewind($blob);

$id = 1;
$stmt->bindparam(':id', $id);
$stmt->bindparam(':foo', $blob, PDO::PARAM_LOB);
$stmt->execute();

$res = $db->query("SELECT blob1 from test46274");
// Resource
var_dump($res->fetch());

// Empty string
var_dump($res->fetch());
?>
--CLEAN--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory('ext/pdo_oci/tests/common.phpt');
PDOTest::dropTableIfExists($db, "test46274");
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
