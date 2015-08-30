--TEST--
Bug #62358 (Segfault when using traits a lot)
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
Warning: Declaration of B::foo($var) should be compatible with A::foo() in %sbug62358.php on line %d
