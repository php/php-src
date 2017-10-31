--TEST--
PDO_DBLIB: DATETIME_CONVERT attr
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';
$sql = "SELECT convert(datetime, '20171027 10:22:44.135') AS [d]";

var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_CONVERT));

$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));
$stmt->closeCursor();

//this test assumes FreeTDS's default "date format" = %b %e %Y %I:%M:%S:%z%p
$db->setAttribute(PDO::DBLIB_ATTR_DATETIME_CONVERT, 1);
var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_CONVERT));
$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));
$stmt->closeCursor();
?>
--EXPECT--
bool(false)
array(1) {
  ["d"]=>
  string(19) "2017-10-27 10:22:44"
}
bool(true)
array(1) {
  ["d"]=>
  string(26) "Oct 27 2017 10:22:44:137AM"
}
