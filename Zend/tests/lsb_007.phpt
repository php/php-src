--TEST--
ZE2 Late Static Binding ensuring implementing 'static' is not allowed
--FILE--
<?php

class Foo implements static {
}

?>
==DONE==
--EXPECTF--
Fatal error: Cannot use 'static' as interface name, as it is reserved in %s on line %d
