--TEST--
Asymmetric visibility private(get) private(set) not allowed
--FILE--
<?php

class Foo {
    private private(set) string $bar;
}

?>
--EXPECTF--
Fatal error: Get visibility of property Foo::$bar must be higher than set visibility in %s on line %d
