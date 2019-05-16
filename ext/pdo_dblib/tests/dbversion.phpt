--TEST--
PDO_DBLIB: \PDO::DBLIB_ATTR_VERSION exposes a string
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require __DIR__ . '/config.inc';
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

$version = $db->getAttribute(PDO::DBLIB_ATTR_VERSION);
var_dump(is_string($version) && strlen($version));

?>
--EXPECT--
bool(true)
