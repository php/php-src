--TEST--
Unpack iterator by reference with type check
--FILE--
<?php
function test(T &$a) {
}
function gen() {
    yield null;
}
try {
    test(...gen());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Warning: Cannot pass by-reference argument 1 of test() by unpacking a Traversable, passing by-value instead in %s on line %d
test(): Argument #1 ($a) must be of type T, null given, called in %s on line %d
