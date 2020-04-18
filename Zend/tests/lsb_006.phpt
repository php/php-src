--TEST--
ZE2 Late Static Binding ensuring extending 'static' is not allowed
--FILE--
<?php

class Foo extends static {
}

?>
==DONE==
--EXPECTF--
Fatal error: Cannot use 'static' as class name, as it is reserved in %s on line %d
