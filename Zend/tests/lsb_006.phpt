--TEST--
ZE2 Late Static Binding ensuring extending 'static' is not allowed
--FILE--
<?php

class Foo extends static {
}

?>
==DONE==
--EXPECTF--
Parse error: %s error,%sexpecting %s in %s on line %d

