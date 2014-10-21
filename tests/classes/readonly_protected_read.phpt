--TEST--
Reading readonly protected properties
--FILE--
<?php

class Foo {
    protected readonly $bar;

    public function __construct() {
        $this->bar = 'elePHPants';
    }
}

class Bar extends Foo {
    public function read(Foo $x) {
        return $x->bar;
    }
}

$x = new Foo;
$y = new Bar;
var_dump($y->read($x));
--EXPECTF--
string(10) "elePHPants"
