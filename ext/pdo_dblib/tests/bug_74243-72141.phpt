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
$sql = "SELECT convert(datetime, '20171027 10:22:44.135') AS [d]";

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

$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));
$stmt->closeCursor();

$db->setAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT, "\u\\fY-m-d H:i:s.\\\\f");
$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));
$stmt->closeCursor();

$db->setAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT, "u = Y-m-d H:i:s");
$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));
$stmt->closeCursor();

$db->setAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT, "Y-m-d u H:i:s");
$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));
$stmt->closeCursor();

$db->setAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT, "");
var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT));
$db->setAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT, "test");
var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT));
$db->setAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT, null);
var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT));
?>
--EXPECT--
bool(false)
NULL
array(1) {
  ["d"]=>
  string(19) "2017-10-27 10:22:44"
}
bool(true)
array(1) {
  ["d"]=>
  string(26) "Oct 27 2017 10:22:44:137AM"
}
array(1) {
  ["d"]=>
  string(19) "2017-10-27 10:22:44"
}
array(1) {
  ["d"]=>
  string(26) "uf2017-10-27 10:22:44.\137"
}
array(1) {
  ["d"]=>
  string(28) "136667 = 2017-10-27 10:22:44"
}
array(1) {
  ["d"]=>
  string(26) "2017-10-27 136667 10:22:44"
}
NULL
string(4) "test"
NULL
