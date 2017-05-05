--TEST--
  PDO_DBLIB: Ensure quote function returns expected results
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';
var_dump($db->quote('foo'));
?>
--EXPECT--
string(6) "N'foo'"
