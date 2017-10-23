--TEST--
PDO_DBLIB: DATETIME_CONVERT and DATETIME_FORMAT attrs
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';
$sql = "SELECT convert(datetime, '20171017 10:22:44.123') AS [d]";

var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_CONVERT));
var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT));

$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));
$stmt->closeCursor();

$db->setAttribute(PDO::DBLIB_ATTR_DATETIME_CONVERT, 1);
var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_CONVERT));
$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));
$stmt->closeCursor();

$db->setAttribute(PDO::DBLIB_ATTR_DATETIME_CONVERT, 0);
$db->setAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT, "%b %e %Y %I:%M:%S:%z%p");
var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT));
$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));
$stmt->closeCursor();
?>
--EXPECT--
bool(false)
string(0) ""
array(1) {
  ["d"]=>
  string(19) "2017-10-17 10:22:44"
}
bool(true)
array(1) {
  ["d"]=>
  string(26) "Oct 17 2017 10:22:44:123AM"
}
string(22) "%b %e %Y %I:%M:%S:%z%p"
array(1) {
  ["d"]=>
  string(26) "Oct 17 2017 10:22:44:123AM"
}
