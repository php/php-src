--TEST--
PFA RFC examples: "Magic methods" section
--FILE--
<?php

require 'rfc_examples.inc';

class Foo {
    public function __call($method, $args) {
        printf("%s::%s\n", __CLASS__, $method);
        print_r($args);
    }
}

$f = new Foo();

check_equivalence([
    'Test 1' => [
        $f->method(?, ?),
        (function ($f) {
            return fn($args0, $args1) => $f->method($args0, $args1);
        })($f)->bindTo($f),
    ],
]);

try {
    $f->method(?, ?)(a: 1, b: 2);
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

?>
--EXPECT--
# Test 1: Foo::method
Array
(
    [0] => mixed(0)
    [1] => mixed(1)
)
Foo::method
Array
(
    [0] => mixed(0)
    [1] => mixed(1)
)
Ok
Error: Unknown named parameter $a
