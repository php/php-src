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

--EXPECTF--
Fatal error: Declaration of Bar::test(): B must be compatible with Foo::test(): A in %sinheritance006.php on line 14
