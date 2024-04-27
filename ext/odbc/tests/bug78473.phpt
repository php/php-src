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
--EXPECT--
odbc_close(): Argument #1 ($odbc) must be of type Odbc\Connection, resource given
resource(1) of type (stream)
