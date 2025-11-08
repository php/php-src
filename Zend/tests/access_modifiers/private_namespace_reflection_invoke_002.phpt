--TEST--
private(namespace) method via ReflectionMethod::invoke - different namespace requires setAccessible
--FILE--
<?php

namespace Foo {
    class A {
        private(namespace) function test($arg) {
            return "A::test: $arg";
        }
    }
}

namespace Bar {
    $obj = new \Foo\A();
    $method = new \ReflectionMethod(\Foo\A::class, 'test');

    try {
        // Without setAccessible - should fail
        var_dump($method->invoke($obj, 'hello'));
    } catch (\ReflectionException $e) {
        echo "Expected: " . $e->getMessage() . "\n";
    }

    // With setAccessible - should work
    $method->setAccessible(true);
    var_dump($method->invoke($obj, 'hello'));
}

?>
--EXPECTF--
Expected: Cannot invoke private(namespace) method Foo\A::test() from namespace Bar
string(14) "A::test: hello"
