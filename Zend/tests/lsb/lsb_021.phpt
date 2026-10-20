--TEST--
ZE2 Late Static Binding parent::/self:: forwarding while classname doesn't
--FILE--
<?php
class A {
    public static function test() {
        echo get_called_class()."\n";
    }
}

class B extends A {
    public static function testForward() {
        parent::test();
        call_user_func("parent::test");
        call_user_func(array("parent", "test"));
        self::test();
        call_user_func("self::test");
        call_user_func(array("self", "test"));
    }
    public static function testNoForward() {
        A::test();
        call_user_func("A::test");
        call_user_func(array("A", "test"));
        B::test();
        call_user_func("B::test");
        call_user_func(array("B", "test"));

        (self::class)::test();
        call_user_func(self::class . "::test");
        call_user_func(array(self::class, "test"));
    }
}

class C extends B {

}

C::testForward();
C::testNoForward();

?>
--EXPECTF--
C

Deprecated: Use of "parent" in callables is deprecated in %s on line %d
C

Deprecated: Use of "parent" in callables is deprecated in %s on line %d
C
C

Deprecated: Use of "self" in callables is deprecated in %s on line %d
C

Deprecated: Use of "self" in callables is deprecated in %s on line %d
C
A
A
A
B
B
B
B
B
B
