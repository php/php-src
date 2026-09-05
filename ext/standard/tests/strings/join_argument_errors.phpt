--TEST--
Test join() - argument errors
--FILE--
<?php

foreach ([
    fn() => join(STDIN, []),
    fn() => join(',', new stdClass()),
] as $callback) {
    try {
        $callback();
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
TypeError: join(): Argument #1 ($separator) must be of type array|string, resource given
TypeError: join(): Argument #2 ($array) must be of type ?array, stdClass given
