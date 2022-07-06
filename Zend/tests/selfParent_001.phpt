--TEST--
Test when constants are initialised. See also selfParent_002.phpt.
--FILE--
<?php
class A {
    const myConst = "const in A";
    const myDynConst = self::myConst;

    public static function test() {
        var_dump(self::myDynConst);
    }
}

class B extends A {
    const myConst = "const in B";

    public static function test() {
        var_dump(parent::myDynConst);
    }
}

A::test();
B::test();
?>
--EXPECT--
string(10) "const in A"
string(10) "const in A"
