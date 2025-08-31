--TEST--
unregister_tick_function only accepts a valid callback as parameter
--FILE--
<?php
declare(ticks=1);

try {
    unregister_tick_function("a");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
unregister_tick_function(): Argument #1 ($callback) must be a valid callback, function "a" not found or invalid function name
