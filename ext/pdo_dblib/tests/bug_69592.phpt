--TEST--
PDO_DBLIB: Empty resultsets on SET NOCOUNT expression
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';

$sql = '
    SET NOCOUNT ON
    SELECT 0 AS [result]
';

$stmt = $db->query($sql);
var_dump($stmt->fetchAll(PDO::FETCH_ASSOC)); // expected: array(result => 0), actual: array()
var_dump($stmt->nextRowset()); // expected: bool(false), actual: bool(true)
var_dump($stmt->fetchAll(PDO::FETCH_ASSOC)); // expected: array(), actual: array(result => 0)
?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    ["result"]=>
    string(1) "0"
  }
}
bool(false)
array(0) {
}

