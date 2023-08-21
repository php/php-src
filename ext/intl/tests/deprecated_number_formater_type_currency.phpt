--TEST--
NumberFormatter::TYPE_CURRENCY is deprecated
--EXTENSIONS--
intl
--FILE--
<?php
var_dump(NumberFormatter::TYPE_CURRENCY);
var_dump(constant('NumberFormatter::TYPE_CURRENCY'));
?>
--EXPECTF--
Deprecated: Constant NumberFormatter::TYPE_CURRENCY is deprecated in %s on line %d
int(4)

Deprecated: Constant NumberFormatter::TYPE_CURRENCY is deprecated in %s on line %d
int(4)
