--TEST--
PDO_DBLIB: Does not support get column meta
--EXTENSIONS--
pdo_dblib
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
getDbConnection();
?>
--CONFLICTS--
all
--FILE--
<?php
require __DIR__ . '/config.inc';

$db = getDbConnection();
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
