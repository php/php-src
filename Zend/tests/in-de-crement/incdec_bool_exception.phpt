--TEST--
Inc/dec on bool: warning converted to exception
--FILE--
<?php

set_error_handler(function($severity, $m) {
    throw new Exception($m, $severity);
});

$values = [false, true];
foreach ($values as $value) {
    try {
        $value++;
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    try {
        $value--;
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
Exception: Increment on type bool has no effect, this will change in the next major version of PHP
Exception: Decrement on type bool has no effect, this will change in the next major version of PHP
Exception: Increment on type bool has no effect, this will change in the next major version of PHP
Exception: Decrement on type bool has no effect, this will change in the next major version of PHP
