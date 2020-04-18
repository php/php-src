--TEST--
Closure 036: Rebinding closures, keep calling scope
--FILE--
<?php

class A {
    private $x;

    public function __construct($v) {
        $this->x = $v;
    }

    public function getIncrementor() {
        return function() { return ++$this->x; };
    }
}

$a = new A(0);
$b = new A(10);

$ca = $a->getIncrementor();
$cb = $ca->bindTo($b);
$cb2 = Closure::bind($ca, $b);

var_dump($ca());
var_dump($cb());
var_dump($cb2());

?>
--EXPECT--
int(1)
int(11)
int(12)
