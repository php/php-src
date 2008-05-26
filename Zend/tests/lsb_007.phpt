--TEST--
ZE2 Late Static Binding ensuring implementing 'static' is not allowed
--FILE--
<?php

class Foo implements static {
}

?>
==DONE==
--EXPECTF--
Parse error: parse error, expecting `T_STRING' or `T_PAAMAYIM_NEKUDOTAYIM' or `T_NAMESPACE' in %slsb_007.php on line %d