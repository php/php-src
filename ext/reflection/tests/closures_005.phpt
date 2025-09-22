--TEST--
Test closure isStatic
--FILE--
<?php

namespace {
    class Foo {
        public static function bar(){}
        public function baz() {}

        public function qux() {
            return static function(){};
        }
    }

    $foo = new Foo;

    var_dump(
        (new ReflectionFunction(function(){}))->isStatic(),
        (new ReflectionFunction(static function(){}))->isStatic(),
        (new ReflectionFunction($foo->qux()))->isStatic(),
        (new ReflectionMethod($foo, 'bar'))->isStatic(),
        (new ReflectionMethod($foo, 'baz'))->isStatic(),

        // Static closure inference exceptions
        (new ReflectionFunction(function () { $$var; }))->isStatic(),
        (new ReflectionFunction(function () { $var(); }))->isStatic(),
        (new ReflectionFunction(function () { call_user_func('Foo::bar'); }))->isStatic(),
        (new ReflectionFunction(function () { ('Foo::bar')(); }))->isStatic(),
        (new ReflectionFunction(function () { Foo::bar(); }))->isStatic(),
        (new ReflectionFunction(function () { $this; }))->isStatic(),
        (new ReflectionFunction(function () { function () { $this; }; }))->isStatic(),
        (new ReflectionFunction(function () { function () { require $var; }; }))->isStatic(),
        (new ReflectionFunction(function () { function () { eval('Foo::bar();'); }; }))->isStatic(),
    );
}

namespace Foo {
    var_dump(
        // Static closure inference exceptions
        (new \ReflectionFunction(function () { call_user_func('Foo::bar'); }))->isStatic(),
        (new \ReflectionFunction(function () { ('Foo::bar')(); }))->isStatic(),
        (new \ReflectionFunction(function () { Foo::bar(); }))->isStatic(),
    );
}

?>
--EXPECT--
bool(true)
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
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
