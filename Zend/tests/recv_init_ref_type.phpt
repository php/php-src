--TEST--
Check by-ref RECV_INIT with single type
--FILE--
<?php

function test(array &$foo = []) {
}

try {
    $bar = 42;
    test($bar);
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
TypeError: test(): Argument #1 ($foo) must be of type array, int given, called in %s on line %d
