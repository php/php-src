--TEST--
PDO_DBLIB: Return date as DateTime object
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';
$sql = "SELECT convert(datetime, '20171017 10:22:44.123') AS [d]";

//init = DEFAULT
var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT) == PDO::DBLIB_DATETIME_FORMAT_DEFAULT);

//DEFAULT, expected = ISO format
$db->setAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT, PDO::DBLIB_DATETIME_FORMAT_DEFAULT);
$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));
$stmt->closeCursor();

//MILLISECOND, expected = ISO format + ms
$db->setAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT, PDO::DBLIB_DATETIME_FORMAT_MILLISECOND);
var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT));
$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));
$stmt->closeCursor();

//CONVERT, expected = freetds format
//FIXME: this test assumes we're using freetds default "date format" = %b %e %Y %I:%M:%S:%z%p
$db->setAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT, PDO::DBLIB_DATETIME_FORMAT_CONVERT);
var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT));
$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));
$stmt->closeCursor();

?>
--EXPECT--
bool(true)
array(1) {
  ["d"]=>
  string(19) "2017-10-17 10:22:44"
}
int(1)
array(1) {
  ["d"]=>
  string(23) "2017-10-17 10:22:44.123"
}
int(2)
array(1) {
  ["d"]=>
  string(26) "Oct 17 2017 10:22:44:123AM"
}
