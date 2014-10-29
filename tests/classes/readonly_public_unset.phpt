--TEST--
Unsetting readonly public properties
--FILE--
<?php

class Foo {
    public readonly $foo;
    public readonly $bar;
    public readonly $qux;

    public function __construct() {
        unset($this->foo);
    }
}

class Bar extends Foo {
    public function changeBar(Foo $x) {
        unset($x->bar);
    }
}

$x = new Foo;

$y = new Bar;
$y->changeBar($x);

unset($x->qux);
--EXPECTF--
Fatal error: Cannot write to readonly public property Foo::$qux in %s on line %d
