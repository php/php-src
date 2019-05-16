--TEST--
PDO_DBLIB: \PDO::DBLIB_ATTR_TDS_VERSION exposes a string or false
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require __DIR__ . '/config.inc';
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

$version = $db->getAttribute(PDO::DBLIB_ATTR_TDS_VERSION);
var_dump((is_string($version) && strlen($version)) || $version === false);

?>
--EXPECT--
bool(true)
