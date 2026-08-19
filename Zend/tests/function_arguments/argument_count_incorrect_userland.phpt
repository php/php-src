--TEST--
Call userland function with incorrect number of arguments
--FILE--
<?php
try {
    function foo($bar) { }
    foo();
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    function bar($foo, $bar) { }
    bar(1);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

function bat(int $foo, string $bar) { }

try {
    bat(123);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    bat("123");
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
ArgumentCountError: Too few arguments to function foo(), 0 passed in %s and exactly 1 expected
ArgumentCountError: Too few arguments to function bar(), 1 passed in %s and exactly 2 expected
ArgumentCountError: Too few arguments to function bat(), 1 passed in %s and exactly 2 expected
ArgumentCountError: Too few arguments to function bat(), 1 passed in %s and exactly 2 expected
