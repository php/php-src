--TEST--
Bug #79777: String cast exception during die should be handled gracefully
--FILE--
<?php

try {
    die(new stdClass);
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: exit(): Argument #1 ($status) must be of type string|int, stdClass given
