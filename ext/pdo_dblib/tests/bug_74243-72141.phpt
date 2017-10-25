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
$sql = "SELECT convert(datetime, '19231017 10:22:44.135') AS [d]";

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
$stmt = $db->query(str_replace('1923', '2123', $sql));
var_dump($stmt->fetch(PDO::FETCH_ASSOC));
$stmt->closeCursor();

$db->setAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT, "Y-m-d H:i:s.f/u");
var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT));
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
  string(19) "1923-10-17 10:22:44"
}
bool(true)
array(1) {
  ["d"]=>
  string(26) "Oct 17 1923 10:22:44:137AM"
}
array(1) {
  ["d"]=>
  string(19) "1923-10-17 10:22:44"
}
array(1) {
  ["d"]=>
  string(19) "2123-10-17 10:22:44"
}
string(15) "Y-m-d H:i:s.f/u"
array(1) {
  ["d"]=>
  string(30) "1923-10-17 10:22:44.137/136666"
}
NULL
string(4) "test"
NULL
