--TEST--
private(namespace) method visibility with Closure::fromCallable - global namespace
--FILE--
<?php

namespace Foo {
    class A {
        private(namespace) function test() {
            return "A::test";
        }
    }
}

namespace {
    // Global namespace - should fail
    $obj = new \Foo\A();
    try {
        $closure = \Closure::fromCallable([$obj, 'test']);
        var_dump($closure());
    } catch (\TypeError $e) {
        echo $e->getMessage();
    }
}

?>
--EXPECTF--
Failed to create closure from callable: cannot access private(namespace) method Foo\A::test()
