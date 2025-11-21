--TEST--
private(namespace) static method with Closure::fromCallable - different namespace
--FILE--
<?php

namespace Foo {
    class A {
        private(namespace) static function test() {
            return "A::test";
        }
    }
}

namespace Bar {
    // Different namespace - should fail
    try {
        $closure = \Closure::fromCallable([\Foo\A::class, 'test']);
        var_dump($closure());
    } catch (\TypeError $e) {
        echo $e->getMessage();
    }
}

?>
--EXPECTF--
Failed to create closure from callable: cannot access private(namespace) method Foo\A::test()
