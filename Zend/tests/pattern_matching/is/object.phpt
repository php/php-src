--TEST--
Object pattern matching
--FILE--
<?php

class Foo {
    public function __construct(
        public int $a,
    ) {}

    public static function isSelfFoo($value) {
        return $value is self;
    }

    public static function isStaticFoo($value) {
        return $value is static;
    }
}

class Bar extends Foo implements Baz {
    public static function isSelfBar($value) {
        return $value is self;
    }

    public static function isParentBar($value) {
        return $value is parent;
    }
}

interface Baz {}
class Qux {}

$foo = new Foo(42);
$bar = new Bar(43);
$qux = new Qux();

var_dump($foo is Foo);
var_dump($foo is Bar);
var_dump($foo is Baz);
var_dump($foo is Qux);
var_dump($foo is Quux);
var_dump($bar is Foo);
var_dump($bar is Bar);
var_dump($bar is Baz);
var_dump($bar is Qux);
var_dump($bar is Quux);
var_dump(null is Foo);
var_dump(null is Bar);
var_dump(null is Baz);
var_dump(null is Qux);
var_dump(null is Quux);

var_dump($foo is Foo(a: 42));
var_dump($foo is Foo(a: 42|43));
var_dump($foo is Foo(a: 'hello world'));
var_dump($foo is Foo(b: 42));
var_dump($bar is Foo(a: 42));
var_dump($bar is Foo(a: 42|43));
var_dump($bar is Foo(a: 'hello world'));
var_dump($bar is Foo(b: 42));

var_dump(Foo::isSelfFoo($foo));
var_dump(Foo::isSelfFoo($bar));
var_dump(Foo::isSelfFoo($qux));
var_dump(Foo::isStaticFoo($foo));
var_dump(Foo::isStaticFoo($bar));
var_dump(Foo::isStaticFoo($qux));
var_dump(Bar::isSelfFoo($foo));
var_dump(Bar::isSelfFoo($bar));
var_dump(Bar::isSelfFoo($qux));
var_dump(Bar::isStaticFoo($foo));
var_dump(Bar::isStaticFoo($bar));
var_dump(Bar::isStaticFoo($qux));
var_dump(Bar::isSelfBar($foo));
var_dump(Bar::isSelfBar($bar));
var_dump(Bar::isSelfBar($qux));
var_dump(Bar::isParentBar($foo));
var_dump(Bar::isParentBar($bar));
var_dump(Bar::isParentBar($qux));

?>
--EXPECTF--
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)

Warning: Undefined property: Foo::$b in %s on line %d
bool(false)
bool(false)
bool(true)
bool(false)

Warning: Undefined property: Bar::$b in %s on line %d
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
