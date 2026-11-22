--TEST--
GH-16643: Incompatible trait property error message states the reason
--FILE--
<?php

trait FooTrait {
    private array $options = [];
}

class Foo {
    use FooTrait;

    public function __construct(private array $options = []) {}
}

?>
--EXPECTF--
Fatal error: Foo and FooTrait define the same property ($options) in the composition of Foo. However, the definition differs and is considered incompatible: only one of them has a default value. Class was composed in %s on line %d
