--TEST--
Asymmetric visibility private(get) protected(set) not allowed
--FILE--
<?php

class Foo {
    private protected(set) string $bar;
}

?>
--EXPECTF--
Fatal error: Visibility of property Foo::$bar must not be weaker than set visibility in %s on line %d
