--TEST--
Writing readonly public properties
--FILE--
<?php

class Foo {
    public readonly $bar;

    public function __construct() {
        $this->bar = 'elePHPants';
    }
}

class Bar extends Foo {
    public function changeBar(Foo $x) {
        $x->bar = 'Monty';
    }
}

$x = new Foo;

$y = new Bar;
$y->changeBar($x);
var_dump($x->bar);

$x = new Foo;
$x->bar = 'Duke';
--EXPECTF--
string(5) "Monty"

Fatal error: Cannot write to readonly public property Foo::$bar in %s on line %d
