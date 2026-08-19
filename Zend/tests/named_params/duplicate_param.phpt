--TEST--
Duplicate param
--FILE--
<?php

function test($a) {}

try {
    test(a: 1, a: 2);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    test(1, a: 2);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Named parameter $a overwrites previous argument
Error: Named parameter $a overwrites previous argument
