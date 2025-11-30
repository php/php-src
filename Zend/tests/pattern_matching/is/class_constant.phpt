--TEST--
Class constant pattern
--FILE--
<?php

class Foo {
    const A = 'a';
    private const B = 'b';

    public static function test() {
        var_dump('b' is self::B);
        var_dump('c' is self::B);
    }
}
enum Bar {
    case A;
}

var_dump('a' is Foo::A);
var_dump('b' is Foo::A);
try {
    var_dump('a' is Foo::B);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump('a' is Foo::C);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
Foo::test();
var_dump(Bar::A is Bar::A);
var_dump('foo' is Bar::A);

?>
--EXPECT--
bool(true)
bool(false)
Error: Cannot access private constant Foo::B
Error: Undefined constant Foo::C
bool(true)
bool(false)
bool(true)
bool(false)
