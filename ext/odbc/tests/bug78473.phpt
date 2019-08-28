--TEST--
Bug #78473 (odbc_close() closes arbitrary resources)
--SKIPIF--
<?php
if (!extension_loaded('odbc')) die('skip odbc extension not available');
?>
--FILE--
<?php
odbc_close(STDIN);
var_dump(STDIN);
?>
--EXPECTF--
Warning: odbc_close(): supplied resource is not a valid ODBC-Link resource in %s on line %d
resource(%d) of type (stream)
