--TEST--
Trait compatibility of readonly properties
--FILE--
<?php

trait Foo {
    public $x;
}

class Bar {
    use Foo;
    public readonly $x;
}
--EXPECTF--
Fatal error: Bar and Foo define the same property ($x) in the composition of Bar. However, the definition differs and is considered incompatible. Class was composed in %s on line %d
