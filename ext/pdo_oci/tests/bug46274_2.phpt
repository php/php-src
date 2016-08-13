--TEST--
Bug #46274 (pdo_pgsql - Segfault when using PDO::ATTR_STRINGIFY_FETCHES and blob)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) 
die('skip not loaded');
require dirname(__FILE__).'/../../pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory('ext/pdo_oci/tests/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

try {
	$db->exec("DROP TABLE test_one_blob");
} catch (Exception $e) {
}

$db->beginTransaction();

$db->query('CREATE TABLE test_one_blob (id INT NOT NULL, blob1 BLOB)');

$stmt = $db->prepare("INSERT INTO test_one_blob (id, blob1) VALUES (:id, EMPTY_BLOB()) RETURNING blob1 INTO :foo");

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

$res = $db->query("SELECT blob1 from test_one_blob");
// Resource
var_dump($row = $res->fetch());
var_dump(fread($row[0], 1024));
fclose($row[0]);

// Empty string
var_dump($row = $res->fetch());
var_dump(fread($row[0], 1024));
fclose($row[0]);

$db->exec("DROP TABLE test_one_blob");

?>
--EXPECTF--
array(2) {
  ["blob1"]=>
  resource(%d) of type (stream)
  [0]=>
  resource(%d) of type (stream)
}
string(3) "foo"
array(2) {
  ["blob1"]=>
  resource(%d) of type (stream)
  [0]=>
  resource(%d) of type (stream)
}
string(0) ""
