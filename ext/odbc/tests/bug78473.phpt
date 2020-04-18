--TEST--
Bug #78473 (odbc_close() closes arbitrary resources)
--SKIPIF--
<?php
if (!extension_loaded('odbc')) die('skip odbc extension not available');
?>
--FILE--
<?php
try {
    odbc_close(STDIN);
} catch (TypeError $err) {
    echo $err->getMessage(), PHP_EOL;
}
var_dump(STDIN);
?>
--EXPECTF--
odbc_close(): supplied resource is not a valid ODBC-Link resource
resource(%d) of type (stream)
