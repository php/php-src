--TEST--
Typed class constants (declaration; enums)
--FILE--
<?php
enum E {
    const E FooConst = E::Foo;

    case Foo;
}

class A {
    public const E CONST1 = E::Foo;
    public const E CONST2 = E::FooConst;
}

var_dump(E::FooConst);
var_dump(A::CONST1);
var_dump(A::CONST2);
?>
--EXPECTF--
enum(E::Foo)
enum(E::Foo)
enum(E::Foo)
