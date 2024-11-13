--TEST--
Asymmetric visibility private(get) protected(set) in CPP is not allowed
--FILE--
<?php

class Foo {
    public function __construct(
        private protected(set) string $bar
    ) {}
}

?>
--EXPECTF--
Fatal error: Visibility of property Foo::$bar must not be weaker than set visibility in %s on line %d
