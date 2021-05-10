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
--EXPECTF--
Deprecated: declare(ticks) is deprecated in %s on line %d

Deprecated: Function register_tick_function() is deprecated in %s on line %d
register_tick_function(): Argument #1 ($callback) must be a valid callback, function "a" not found or invalid function name
