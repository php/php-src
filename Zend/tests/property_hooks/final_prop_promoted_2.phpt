--TEST--
Promoted property may be marked final (normal)
--FILE--
<?php

class A {
    public function __construct(
        public final $prop
    ) {}
}

class B extends A {
    public $prop { get {} set {} }
}

?>
--EXPECTF--
Fatal error: Cannot override final property A::$prop in %s on line %d
