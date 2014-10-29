--TEST--
Referencing readonly protected properties
--FILE--
<?php

class Foo {
    protected readonly $bar;

    public function __construct() {
        $x = &$this->bar;
        $x = 'elePHPants';
    }
}

class Bar extends Foo {
    public function changeBar(Foo $x) {
        var_dump($x->bar);
        $y = &$x->bar;
        $y = 'Monty';
    }
}

$x = new Foo;

$y = new Bar;
$y->changeBar($x);
--EXPECTF--
string(10) "elePHPants"

Fatal error: Cannot write to readonly protected property Foo::$bar in %s on line %d
