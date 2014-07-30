--TEST--
Closure::call
--FILE--
<?php

class Foo {
    public $x = 0;
    function bar() {
        return function () {
            return $this->x;
        };
    }
}

$foo = new Foo;
$qux = $foo->bar();

$foobar = new Foo;
$foobar->x = 3;

var_dump($qux());
var_dump($qux->call($foo));
var_dump($qux->call($foobar));


$bar = function () {
    return $this->x;
};

$elePHPant = new StdClass;
$elePHPant->x = 7;

var_dump($bar->call($elePHPant));


$beta = function ($z) {
    return $this->x * $z;
};

var_dump($beta->call($elePHPant, 3));

?>
--EXPECT--
int(0)
int(0)
int(3)
int(7)
int(21)