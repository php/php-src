--TEST--
GH-7900: Arrow function with never return type compile-time errors
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php

$x = fn(): never => throw new \Exception('Here');

try {
    var_dump($x());
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    assert((fn(): never => 42) && false);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Exception: Here
AssertionError: assert((fn(): never => 42) && false)
