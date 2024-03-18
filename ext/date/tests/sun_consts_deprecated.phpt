--TEST--
SUNFUNCS_RET_STRING, SUNFUNCS_RET_DOUBLE, and SUNFUNCS_RET_TIMESTAMP constants are deprecated
--FILE--
<?php

var_dump(SUNFUNCS_RET_STRING);
var_dump(SUNFUNCS_RET_DOUBLE);
var_dump(SUNFUNCS_RET_TIMESTAMP);

?>
--EXPECTF--
Deprecated: Constant SUNFUNCS_RET_STRING is deprecated in %s on line %d
int(1)

Deprecated: Constant SUNFUNCS_RET_DOUBLE is deprecated in %s on line %d
int(2)

Deprecated: Constant SUNFUNCS_RET_TIMESTAMP is deprecated in %s on line %d
int(0)
