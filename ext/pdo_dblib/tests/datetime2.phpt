--TEST--
PDO_DBLIB: DATETIME2 column data
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require __DIR__ . '/config.inc';
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

$sql = "SELECT convert(datetime2, '10231017 10:22:44.1355318') AS [d]";

var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_CONVERT));

$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));

// the format string from locales.conf won't affect these columns
$db->setAttribute(PDO::DBLIB_ATTR_DATETIME_CONVERT, 1);
var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_CONVERT));
$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));

?>
--EXPECT--
bool(false)
array(1) {
  ["d"]=>
  string(27) "1023-10-17 10:22:44.1355318"
}
bool(true)
array(1) {
  ["d"]=>
  string(27) "1023-10-17 10:22:44.1355318"
}
