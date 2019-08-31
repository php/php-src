--TEST--
Bug #70861 Segmentation fault in pdo_parse_params() during Drupal 8 test suite
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

$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

try {
	@$db->query("SET bytea_output = 'escape'");
} catch (Exception $e) {
}

$db->query('DROP TABLE IF EXISTS test_blob_crash CASCADE');
$db->query('CREATE TABLE test_blob_crash (id SERIAL NOT NULL, blob1 BYTEA)');

class HelloWrapper {
	public function stream_open() { return true; }
	public function stream_eof() { return true; }
	public function stream_read() { return NULL; }
	public function stream_stat() { return array(); }
}
stream_wrapper_register("hello", "HelloWrapper");

$f = fopen("hello://there", "r");

$stmt = $db->prepare("INSERT INTO test_one_blob (blob1) VALUES (:foo)", array(PDO::ATTR_CURSOR => PDO::CURSOR_SCROLL));

$stmt->bindparam(':foo', $f, PDO::PARAM_LOB);
$stmt->execute();

fclose($f);

?>
+++DONE+++
--EXPECTF--
%a
+++DONE+++
