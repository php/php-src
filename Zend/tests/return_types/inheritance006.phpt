--TEST--
External covariant return type of self

--INI--
opcache.enable_cli=1

--FILE--
<?php
require __DIR__ . "/classes.php.inc";

class Foo {
    public static function test() : A {
        return new A;
    }
}

class Bar extends Foo {
    public static function test() : B {
        return new B;
    }
}

var_dump(Bar::test());
var_dump(Foo::test());

--EXPECTF--
Strict Standards: Declaration of Bar::test() should be compatible with Foo::test(): A in %sinheritance006.php on line 14
object(B)#%d (0) {
}
object(A)#%d (0) {
}
