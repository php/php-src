--TEST--
PDO_DBLIB: PDO::DBLIB_ATTR_DATETIME_CONVERT
--EXTENSIONS--
pdo_dblib
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
getDbConnection();
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

$db = getDbConnection();

$sql = "SELECT convert(datetime, '20171027 10:22:44.135') AS [d]";

var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_CONVERT));

$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));

// assume default date format: %b %e %Y %I:%M:%S:%z%p
$db->setAttribute(PDO::DBLIB_ATTR_DATETIME_CONVERT, 1);
var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_CONVERT));
$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));

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
