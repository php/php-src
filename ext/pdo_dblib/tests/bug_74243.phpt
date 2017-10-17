--TEST--
PDO_DBLIB: Add milliseconds in dateformat
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';
$sql = "SELECT convert(datetime, '20171017 10:22:44.123') AS [d]";

var_dump($db->getAttribute(PDO::DBLIB_ATTR_MILLISECOND)); // disabled by default

$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC)); // expected: no ms
$stmt->closeCursor();

$db->setAttribute(PDO::DBLIB_ATTR_MILLISECOND, true);
var_dump($db->getAttribute(PDO::DBLIB_ATTR_MILLISECOND));

$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC)); // expected: ms
$stmt->closeCursor();

$db->setAttribute(PDO::DBLIB_ATTR_MILLISECOND, false);
var_dump($db->getAttribute(PDO::DBLIB_ATTR_MILLISECOND)); // disabled by default

?>
--EXPECT--
bool(false)
array(1) {
  ["d"]=>
  string(19) "2017-10-17 10:22:44"
}
bool(true)
array(1) {
  ["d"]=>
  string(23) "2017-10-17 10:22:44.123"
}
bool(false)
