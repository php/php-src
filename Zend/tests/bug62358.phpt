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
Fatal error: Declaration of B::foo($var) must be compatible with I::foo() in %sbug62358.php on line 17
