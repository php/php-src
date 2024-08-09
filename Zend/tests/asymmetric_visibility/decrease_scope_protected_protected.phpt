--TEST--
Asymmetric visibility protected(get) protected(set) not allowed
--FILE--
<?php

class Foo {
    protected protected(set) string $bar;
}

?>
--EXPECTF--
Fatal error: Get visibility of property Foo::$bar must be higher than set visibility in %s on line %d
