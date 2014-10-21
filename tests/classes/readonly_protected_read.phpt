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
    public function read() {
        return $this->bar;
    }
}

$x = new Bar;
var_dump($x->read());
--EXPECTF--
string(10) "elePHPants"
