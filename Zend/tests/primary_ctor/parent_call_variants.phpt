--TEST--
Primary constructors: extends Base() calls parent, extends Base (no parens) does not
--FILE--
<?php
class WithCtor {
    public int $v = 0;
    public function __construct() { echo "WithCtor::__construct\n"; $this->v = 7; }
}

class A(public int $x) extends WithCtor() {}
$a = new A(1);
var_dump($a->x, $a->v);

class B(public int $x) extends WithCtor {}
$b = new B(2);
var_dump($b->x, $b->v);
?>
--EXPECT--
WithCtor::__construct
int(1)
int(7)
int(2)
int(0)
