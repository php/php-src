--TEST--
Protected visibility test case with a grandparent prototype
--FILE--
<?php

class A {
    protected function test() {}
}
class B extends A {
    public function test2($x) {
        $x->test(); // Uncaught Error: Call to protected method D::test() from scope B
    }
}
class C extends A {
    protected function test() {}
}
class D extends C {
    protected function test() {
        echo "Hello World!\n";
    }
}
(new B)->test2(new D);

?>
--EXPECT--
Hello World!
