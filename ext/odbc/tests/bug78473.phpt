--TEST--
Bug #78473 (odbc_close() closes arbitrary resources)
--EXTENSIONS--
odbc
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
