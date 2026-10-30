--TEST--
Bug #70861 Segmentation fault in pdo_parse_params() during Drupal 8 test suite
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

$db->query('CREATE TABLE test70861 (id SERIAL NOT NULL, blob1 BYTEA)');

class HelloWrapper {
    public function stream_open() { return true; }
    public function stream_eof() { return true; }
    public function stream_read() { return NULL; }
    public function stream_stat() { return array(); }
}
stream_wrapper_register("hello", "HelloWrapper");

$f = fopen("hello://there", "r");

$stmt = $db->prepare("INSERT INTO test70861 (blob1) VALUES (:foo)", array(PDO::ATTR_CURSOR => PDO::CURSOR_SCROLL));

$stmt->bindparam(':foo', $f, PDO::PARAM_LOB);
$stmt->execute();

fclose($f);

?>
+++DONE+++
--CLEAN--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->query('DROP TABLE IF EXISTS test70861 CASCADE');
?>
--EXPECTF--
%a
+++DONE+++
