--TEST--
Use of parent inside a class that has / has no parent (success cases)
--FILE--
<?php

// Legal: A2::parent == P2
class P2 {}
class A2 extends P2 {
    public function method(parent $x) {}
}
class B2 extends A2 {
    public function method(P2 $x) {}
}

// Legal: B3::parent == A3 is subclass of A3::parent == P3 in covariant position
class P3 {}
class A3 extends P3 {
    public function method($x): parent {}
}
class B3 extends A3 {
    public function method($x): parent {}
}

?>
===DONE===
--EXPECT--
===DONE===
