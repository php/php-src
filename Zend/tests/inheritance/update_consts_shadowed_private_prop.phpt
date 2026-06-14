--TEST--
Constant updating for shadowed private property
--FILE--
<?php
class Foo {
    private $prop = X;
    function test() {
        var_dump($this->prop);
    }
}

class Bar extends Foo {
    protected $prop;
}

define('X', 1);
$bar = new Bar;
$bar->test();

?>
--EXPECT--
int(1)
