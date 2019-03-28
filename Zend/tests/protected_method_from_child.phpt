--TEST--
Cannot access protected method from child
--FILE--
<?php

class A {
    public function test() {
        var_dump($this->method());
    }
}

class B extends A {
    protected function method() {}
}

(new B)->test();

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to protected method B::method() from context 'A' in %s:%d
Stack trace:
#0 %s(%d): A->test()
#1 {main}
  thrown in %s on line %d
