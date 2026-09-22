--TEST--
register_tick_function only accepts a valid callback as parameter
--FILE--
<?php
declare(ticks=1);

try {
    register_tick_function("a");
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: register_tick_function(): Argument #1 ($callback) must be a valid callback, function "a" not found or invalid function name
