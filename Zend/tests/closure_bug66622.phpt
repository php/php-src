--TEST--
Bug 66622: Closures do not correctly capture the late bound class (static::) in some cases
--FILE--
<?php
class A {
    static function name() { return 'A'; }
    function foo() {
        $fn = function() { return static::name(); };
        echo static::name() . ' vs ' . $fn() . "\n";
    }
    function bar() {
        $fn = static function() { return static::name(); };
        echo static::name() . ' vs ' . $fn() . "\n";
    }
    static function baz() {
        $fn = function() { return static::name(); };
        echo static::name() . ' vs ' . $fn() . "\n";
    }
}
class B extends A {
    static function name() { return 'B'; }
}

function test() {
    (new B)->foo();
    (new B)->bar();
    (new B)->baz();
    B::baz();
}
test();
--EXPECT--
B vs B
B vs B
B vs B
B vs B
