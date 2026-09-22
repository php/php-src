--TEST--
Bug #78344: Segmentation fault on zend_check_protected
--FILE--
<?php

class A {
    protected const FOO = 1;
}

class B {}
class C extends B {
    public function method() {
        var_dump(A::FOO);
    }
}
(new C)->method();

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot access protected constant A::FOO in %s:%d
Stack trace:
#0 %s(%d): C->method()
#1 {main}
  thrown in %s on line %d
