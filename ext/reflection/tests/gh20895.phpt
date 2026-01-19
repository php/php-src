--TEST--
GH-20895: ReflectionProperty does not return the PHPDoc of a property if it contains an attribute with a Closure
--FILE--
<?php

/** Foo */
#[Attr(
    /** Closure 1 */
    static function() { },
    /** Closure 2 */
    static function() { },
)]
class Foo {
    /** Foo::$bar */
    #[Attr(
        /** Closure 3 */
        static function() { },
        /** Closure 4 */
        static function() { },
    )]
    #[Attr(
        /** Closure 5 */
        static function() { },
    )]
    public $bar;

    /** Foo::bar() */
    #[Attr(
        /** Closure 6 */
        static function() { },
    )]
    public function bar() { }
}

/** foo() */
#[Attr(
    /** Closure 7 */
    static function() { },
)]
function foo() { }

#[Attr(
    /** Closure 8 */
    static function() { },
)]
/** bar() */
function bar() { }

/** baz() */
#[Attr(
    static function() { },
)]
function baz() { }

var_dump((new ReflectionClass(Foo::class))->getDocComment());
foreach ((new ReflectionClass(Foo::class))->getAttributes() as $attribute) {
    foreach ($attribute->getArguments() as $argument) {
        var_dump((new ReflectionFunction($argument))->getDocComment());
    }
}
var_dump((new ReflectionProperty(Foo::class, 'bar'))->getDocComment());
foreach ((new ReflectionProperty(Foo::class, 'bar'))->getAttributes() as $attribute) {
    foreach ($attribute->getArguments() as $argument) {
        var_dump((new ReflectionFunction($argument))->getDocComment());
    }
}
var_dump((new ReflectionMethod(Foo::class, 'bar'))->getDocComment());
foreach ((new ReflectionMethod(Foo::class, 'bar'))->getAttributes() as $attribute) {
    foreach ($attribute->getArguments() as $argument) {
        var_dump((new ReflectionFunction($argument))->getDocComment());
    }
}
var_dump((new ReflectionFunction('foo'))->getDocComment());
foreach ((new ReflectionFunction('foo'))->getAttributes() as $attribute) {
    foreach ($attribute->getArguments() as $argument) {
        var_dump((new ReflectionFunction($argument))->getDocComment());
    }
}
var_dump((new ReflectionFunction('bar'))->getDocComment());
foreach ((new ReflectionFunction('bar'))->getAttributes() as $attribute) {
    foreach ($attribute->getArguments() as $argument) {
        var_dump((new ReflectionFunction($argument))->getDocComment());
    }
}
var_dump((new ReflectionFunction('baz'))->getDocComment());
foreach ((new ReflectionFunction('baz'))->getAttributes() as $attribute) {
    foreach ($attribute->getArguments() as $argument) {
        var_dump((new ReflectionFunction($argument))->getDocComment());
    }
}

?>
--EXPECT--
string(10) "/** Foo */"
string(16) "/** Closure 1 */"
string(16) "/** Closure 2 */"
string(16) "/** Foo::$bar */"
string(16) "/** Closure 3 */"
string(16) "/** Closure 4 */"
string(16) "/** Closure 5 */"
string(17) "/** Foo::bar() */"
string(16) "/** Closure 6 */"
string(12) "/** foo() */"
string(16) "/** Closure 7 */"
string(12) "/** bar() */"
string(16) "/** Closure 8 */"
string(12) "/** baz() */"
bool(false)
