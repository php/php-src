--TEST--
lcg_value() deprecation
--FILE--
<?php

var_dump(lcg_value());

?>
--EXPECTF--
Deprecated: Function lcg_value() is deprecated since 8.4, use \Random\Randomizer::getFloat() instead in %s on line %d
float(%f)
