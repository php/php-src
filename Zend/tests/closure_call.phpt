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

// Try on an object other than the one already bound
var_dump($qux->call($foobar));


$bar = function () {
    return $this->x;
};

$elePHPant = new StdClass;
$elePHPant->x = 7;

// Try on a StdClass
var_dump($bar->call($elePHPant));


$beta = function ($z) {
    return $this->x * $z;
};

// Ensure argument passing works
var_dump($beta->call($foobar, 7));

// Ensure ->call calls with scope of passed object
class FooBar {
    private $x = 3;
}

$foo = function () {
    var_dump($this->x);
};

$foo->call(new FooBar);

?>
--EXPECTF--
int(0)
int(0)
int(3)

Warning: Cannot bind closure to scope of internal class stdClass in %s line %d
NULL
int(21)
int(3)
