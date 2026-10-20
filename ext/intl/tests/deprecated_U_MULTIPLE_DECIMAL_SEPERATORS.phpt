--TEST--
U_MULTIPLE_DECIMAL_SEPERATORS is deprecated
--EXTENSIONS--
intl
--FILE--
<?php
var_dump(U_MULTIPLE_DECIMAL_SEPERATORS);
var_dump(constant('U_MULTIPLE_DECIMAL_SEPERATORS'));
?>
--EXPECTF--
Deprecated: Constant U_MULTIPLE_DECIMAL_SEPERATORS is deprecated since 8.3, use U_MULTIPLE_DECIMAL_SEPARATORS instead in %s on line %d
int(65793)

Deprecated: Constant U_MULTIPLE_DECIMAL_SEPERATORS is deprecated since 8.3, use U_MULTIPLE_DECIMAL_SEPARATORS instead in %s on line %d
int(65793)
