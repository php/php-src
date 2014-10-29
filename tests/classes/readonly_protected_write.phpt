--TEST--
Writing readonly protected properties
--FILE--
<?php

class Foo {
    protected readonly $bar;

    public function __construct() {
        $this->bar = 'elePHPants';
    }
}

class Bar extends Foo {
    public function changeBar(Foo $x) {
        var_dump($x->bar);
        $x->bar = 'Monty';
    }
}

$x = new Foo;

$y = new Bar;
$y->changeBar($x);
--EXPECTF--
string(10) "elePHPants"

Fatal error: Cannot write to readonly protected property Foo::$bar in %s on line %d
