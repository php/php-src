--TEST--
PDO_DBLIB: DATETIME2 column data
--EXTENSIONS--
pdo_dblib
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
$db = getDbConnection();
if (in_array($db->getAttribute(PDO::DBLIB_ATTR_TDS_VERSION), ['4.2', '4.6', '5.0', '6.0', '7.0', '7.1', '7.2'])) die('skip feature unsupported by this TDS version');
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

$db = getDbConnection();

$sql = "SELECT convert(datetime2, '10231017 10:22:44.1355318') AS [d]";

var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_CONVERT));

$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));

$db->setAttribute(PDO::DBLIB_ATTR_DATETIME_CONVERT, 1);
var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_CONVERT));
$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));

?>
--EXPECT--
bool(false)
array(1) {
  ["d"]=>
  string(19) "1023-10-17 10:22:44"
}
bool(true)
array(1) {
  ["d"]=>
  string(30) "Oct 17 1023 10:22:44:1355318AM"
}
