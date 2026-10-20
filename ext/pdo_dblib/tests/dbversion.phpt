--TEST--
PDO_DBLIB: \Pdo\Dblib::ATTR_VERSION exposes a string
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

$version = $db->getAttribute(Pdo\Dblib::ATTR_VERSION);
var_dump(is_string($version) && strlen($version));

?>
--EXPECT--
bool(true)
