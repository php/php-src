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
Invalid tick callback 'a' passed
