--TEST--
PDO_DBLIB: Does not support get column meta
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
try {
	$db = new PDO( getenv('PDOTEST_DSN'), getenv('PDOTEST_USER'), getenv('PDOTEST_PASS'));
} catch (PDOException $e) {
	die('skip ' . $e->getMessage());
}
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';
$db = new PDO( getenv('PDOTEST_DSN'), getenv('PDOTEST_USER'), getenv('PDOTEST_PASS'));
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

$stmt = $db->prepare("select ic1.* from information_schema.columns ic1");
$stmt->execute();
var_dump($stmt->getColumnMeta(0));
$stmt = null;
?>
--EXPECT--
array(8) {
  ["max_length"]=>
  int(255)
  ["precision"]=>
  int(0)
  ["scale"]=>
  int(0)
  ["column_source"]=>
  string(13) "TABLE_CATALOG"
  ["native_type"]=>
  string(4) "char"
  ["name"]=>
  string(13) "TABLE_CATALOG"
  ["len"]=>
  int(255)
  ["pdo_type"]=>
  int(2)
}
