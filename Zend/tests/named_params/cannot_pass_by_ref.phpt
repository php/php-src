--TEST--
Cannot pass by reference error with named parameters
--FILE--
<?php
function test($a, &$e) {}
try {
    test(e: 42);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: test(): Argument #2 ($e) could not be passed by reference
