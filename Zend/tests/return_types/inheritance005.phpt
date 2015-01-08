--TEST--
Internal covariant return type of self

--FILE--
<?php
class Foo {
    public static function test() : self {
        return new Foo;
    }
}

class Bar extends Foo {
    public static function test() : self {
        return new Bar;
    }
}

var_dump(Bar::test());
var_dump(Foo::test());

--EXPECTF--
Strict Standards: Declaration of Bar::test() should be compatible with Foo::test(): Foo in %sinheritance005.php on line 12
object(Bar)#%d (0) {
}
object(Foo)#%d (0) {
}
