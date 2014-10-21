--TEST--
Referencing readonly public properties
--FILE--
<?php

class Foo {
    public readonly $bar;

    public function __construct() {
        $x = &$this->bar;
        $x = 'elePHPants';
    }
}

class Bar extends Foo {
    public function changeBar() {
        $x = &$this->bar;
        $x = 'Monty';
    }
}

$y = new Bar;
$y->changeBar();
var_dump($y->bar);

$x = new Foo;
$y = &$x->bar;
--EXPECTF--
string(5) "Monty"

Fatal error: Cannot write to readonly public property Foo::$bar in %s on line %d
