--TEST--
private(namespace) method via variable function call - same namespace
--FILE--
<?php

namespace Foo;

class A {
    private(namespace) static function test() {
        return "A::test";
    }
}

// Same namespace - should work
$callable = [A::class, 'test'];
var_dump($callable());

?>
--EXPECT--
string(7) "A::test"
