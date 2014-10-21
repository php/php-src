--TEST--
Reading readonly public properties
--FILE--
<?php

class Foo {
    public readonly $bar;

    public function __construct() {
        $this->bar = 'elePHPants';
    }
}

$x = new Foo;
var_dump($x->bar);
var_dump((object)$x);
--EXPECTF--
string(10) "elePHPants"
object(Foo)#%d (%d) {
  ["bar"]=>
  string(10) "elePHPants"
}
