--TEST--
Cannot pass by reference error with named parameters
--FILE--
<?php
function test($a, &$e) {}
try {
    test(e: 42);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
test(): Argument #2 ($e) cannot be passed by reference
