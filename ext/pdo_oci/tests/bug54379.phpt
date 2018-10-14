--TEST--
Bug #54379 (PDO_OCI: UTF-8 output gets truncated)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci'))
die('skip not loaded');
require dirname(__FILE__).'/../../pdo/tests/pdo_test.inc';
if (!preg_match('/charset=.*utf8/i', getenv('PDOTEST_DSN')))
die('skip not UTF8 DSN');
PDOTest::skip();
?>
--FILE--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory('ext/pdo_oci/tests/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
try {
        $db->exec("DROP TABLE test");
} catch (Exception $e) {
}
$db->exec("CREATE TABLE test (col1 NVARCHAR2(20))");
$db->exec("INSERT INTO test VALUES('12345678901234567890')");
$db->exec("INSERT INTO test VALUES('あいうえおかきくけこさしすせそたちつてと')");
$stmt = $db->prepare("SELECT * FROM test");
$stmt->execute();
var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
$db->exec("DROP TABLE test");
?>
--EXPECTF--
array(2) {
  [0]=>
  array(1) {
    ["col1"]=>
    string(20) "12345678901234567890"
  }
  [1]=>
  array(1) {
    ["col1"]=>
    string(60) "あいうえおかきくけこさしすせそたちつてと"
  }
}
