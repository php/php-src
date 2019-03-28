--TEST--
Cannot access protected property from child
--FILE--
<?php

class A {
    public function test() {
        var_dump($this->prop);
    }
}

class B extends A {
    protected $prop;
}

(new B)->test();

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot access protected property B::$prop in %s:%d
Stack trace:
#0 %s(%d): A->test()
#1 {main}
  thrown in %s on line %d
