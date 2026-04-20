--TEST--
Test closure static inference
--FILE--
<?php

namespace {
    class Foo {
        public static function bar(){}
    }

    var_dump(
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
