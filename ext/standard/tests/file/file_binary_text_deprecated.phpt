--TEST--
The FILE_BINARY and FILE_TEXT constants are deprecated
--FILE--
<?php

var_dump(FILE_BINARY);
var_dump(FILE_TEXT);

?>
--EXPECTF--
Deprecated: Constant FILE_BINARY is deprecated since 8.1, as the constant has no effect in %s on line %d
int(0)

Deprecated: Constant FILE_TEXT is deprecated since 8.1, as the constant has no effect in %s on line %d
int(0)
