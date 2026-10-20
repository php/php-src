--TEST--
Test deprecation emitted on accessing DOM_PHP_ERR
--EXTENSIONS--
dom
--FILE--
<?php
var_dump(DOM_PHP_ERR);
?>
--EXPECTF--
Deprecated: Constant DOM_PHP_ERR is deprecated since 8.4, as it is no longer used in %s on line %d
int(0)
