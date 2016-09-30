--TEST--
PDO_DBLIB: Does not support get column meta
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';

$stmt = $db->prepare("select top 1 ic1.* from information_schema.columns ic1");
$stmt->execute();
var_dump($stmt->getColumnMeta(0));
$stmt = null;
?>
--EXPECTF--
array(10) {
  ["max_length"]=>
  int(%d)
  ["precision"]=>
  int(0)
  ["scale"]=>
  int(0)
  ["column_source"]=>
  string(13) "TABLE_CATALOG"
  ["native_type"]=>
  string(4) "char"
  ["native_type_id"]=>
  int(%d)
  ["native_usertype_id"]=>
  int(%d)
  ["pdo_type"]=>
  int(2)
  ["name"]=>
  string(13) "TABLE_CATALOG"
  ["len"]=>
  int(%d)
}
