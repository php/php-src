--TEST--
ZE2 Late Static Binding ensuring extending 'static' is not allowed
--FILE--
<?php

class Foo extends static {
}

?>
==DONE==
--EXPECTF--
Fatal error: Cannot use "static" as class name, static is a reserved class name in %s on line %d
