--TEST--
register_tick_function only accepts a valid callback as parameter
--FILE--
<?php
declare(ticks=1);

try {
    register_tick_function("a");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
register_tick_function(): Argument #1 ($callback) must be a valid tick callback, "a" given
