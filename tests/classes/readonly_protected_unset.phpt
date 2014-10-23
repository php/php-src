--TEST--
Unsetting readonly protected properties
--FILE--
<?php

class Foo {
    protected readonly $foo;
    protected readonly $bar;

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
--EXPECTF--
Fatal error: Cannot write to readonly protected property Foo::$bar in %s on line %d
