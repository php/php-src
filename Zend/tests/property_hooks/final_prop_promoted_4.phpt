--TEST--
Final promoted property conflicts with non-promoted non-hooked property
--FILE--
<?php

class A {
    public function __construct(
        final $prop
    ) {}
}

class B extends A {
    public $prop;
}

?>
--EXPECTF--
Fatal error: Cannot override final property A::$prop in %s on line %d
