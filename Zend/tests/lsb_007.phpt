--TEST--
ZE2 Late Static Binding ensuring implementing 'static' is not allowed
--FILE--
<?php

class Foo implements static {
}

?>
==DONE==
--EXPECTF--
Fatal error: Cannot use "static" as interface name, static is a reserved class name in %s on line %d
