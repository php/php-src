--TEST--
Bug #62358 (Segfault when using traits a lot)
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") !== "0") {
    die("skip Need Zend MM enabled");
}
?>
--FILE--
<?php 

trait T {
    public function foo() {
        echo "from T";
    }
}

interface I {
    public function foo();
}

abstract class A implements I{
    use T;
}

class B extends A {
   public function foo($var) {
   } 
}
?>
--EXPECTF--
Strict Standards: Declaration of B::foo() should be compatible with A::foo() in %sbug62358.php on line %d
