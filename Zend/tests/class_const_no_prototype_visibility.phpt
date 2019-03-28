--TEST--
Prototype based visibility checks are not used for class constants
--FILE--
<?php

class A {
    protected const FOO = 'A';
}
class B1 extends A {
    public static function test() {
        var_dump(B2::FOO);
    }
}
class B2 extends A {
    protected const FOO = 'B2';
}

B1::test();

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot access protected const B2::FOO in %s:%d
Stack trace:
#0 %s(%d): B1::test()
#1 {main}
  thrown in %s on line %d
