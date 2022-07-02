--TEST--
forward_static_call() called from outside of a method.
--FILE--
<?php

class A
{
    const NAME = 'A';
    public static function test() {
        echo static::NAME, "\n";
    }
}

class B extends A
{
    const NAME = 'B';

    public static function test() {
        echo self::NAME, "\n";
        forward_static_call(array('parent', 'test'));
    }

    public static function test2() {
        echo self::NAME, "\n";
        forward_static_call(array('self', 'test'));
    }

    public static function test3() {
        echo self::NAME, "\n";
        forward_static_call(array('A', 'test'));
    }
}

class C extends B
{
    const NAME = 'C';

    public static function test()
    {
        echo self::NAME, "\n";
        forward_static_call(array('A', 'test'));
    }
}

A::test();
echo "-\n";
B::test();
echo "-\n";
B::test2();
echo "-\n";
B::test3();
echo "-\n";
C::test();
echo "-\n";
C::test2();
echo "-\n";
C::test3();

?>
--EXPECTF--
A
-
B

Deprecated: Use of "parent" in callables is deprecated in %s on line %d
B
-
B

Deprecated: Use of "self" in callables is deprecated in %s on line %d
B

Deprecated: Use of "parent" in callables is deprecated in %s on line %d
B
-
B
B
-
C
C
-
B

Deprecated: Use of "self" in callables is deprecated in %s on line %d
B

Deprecated: Use of "parent" in callables is deprecated in %s on line %d
C
-
B
C
