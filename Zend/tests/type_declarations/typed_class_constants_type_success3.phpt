--TEST--
Typed enum constants (self/static)
--FILE--
<?php
enum E {
    public const E CONST1 = E::Foo;
    public const self CONST2 = E::Foo;
    public const static CONST3 = E::Foo;
    public const static|stdClass CONST4 = E::Foo;

    case Foo;
}

class A {
    public const E ENUM_CONST = E::Foo;
    public const E CONST1 = E::CONST1;
    public const E CONST2 = E::CONST2;
    public const E CONST3 = E::CONST3;
    public const E CONST4 = E::CONST4;
}

var_dump(A::ENUM_CONST);
var_dump(A::ENUM_CONST);
var_dump(A::CONST1);
var_dump(A::CONST1);
var_dump(A::CONST2);
var_dump(A::CONST2);
var_dump(E::CONST3);
var_dump(E::CONST3);
var_dump(A::CONST3);
var_dump(A::CONST3);
var_dump(E::CONST4);
var_dump(E::CONST4);
var_dump(A::CONST4);
var_dump(A::CONST4);

?>
--EXPECT--
enum(E::Foo)
enum(E::Foo)
enum(E::Foo)
enum(E::Foo)
enum(E::Foo)
enum(E::Foo)
enum(E::Foo)
enum(E::Foo)
enum(E::Foo)
enum(E::Foo)
enum(E::Foo)
enum(E::Foo)
enum(E::Foo)
enum(E::Foo)
