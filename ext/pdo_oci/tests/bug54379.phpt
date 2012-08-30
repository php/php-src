--TEST--
Bug #54379 (PDO_OCI: UTF-8 output gets truncated)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) 
die('skip not loaded');
require dirname(__FILE__).'/../../pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
putenv('PDO_OCI_TEST_DSN', 'oci:dbname=localhost/xe;charset=AL32UTF8');
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory('ext/pdo_oci/tests/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);


try {
	$db->exec("DROP TABLE test_utf8_trunc");
} catch (Exception $e) {
}


$db->exec("CREATE TABLE test_utf8_trunc (col1 NVARCHAR2(20))");
$db->exec("INSERT INTO test_utf8_trunc VALUES('12345678901234567890')");
$db->exec("INSERT INTO test_utf8_trunc VALUES('あいうえおかきくけこさしすせそたちつてと')");

$stmt = $db->prepare("SELECT * FROM test_utf8_trunc");

$stmt->execute();
var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

$db->exec("DROP TABLE test_utf8_trunc");

?>
--EXPECTF--
array(2) {
  [0]=>
  array(1) {
    ["COL1"]=>
    string(20) "12345678901234567890"
  }
  [1]=>
  array(1) {
    ["COL1"]=>
    string(40) "あいうえおかきくけこさしすせそたちつてと"
  }
}
