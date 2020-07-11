--TEST--
Edge cases relating to reference source tracking
--FILE--
<?php

class A {
    public int $prop = 42;
}
class B extends A {
}

$b = new B;
$r =& $b->prop;
unset($b);
$r = "foo"; // ok

class A2 {
    private int $prop = 42;

    public function &getRef() {
        return $this->prop;
    }
}
class B2 extends A2 {
    public $prop;
}

$b2 = new B2;
$r2 =& $b2->getRef();
unset($b2);
$r2 = "foo"; // ok

?>
===DONE===
--EXPECT--
===DONE===
