--TEST--
Test sprintf() - argument errors
--FILE--
<?php

try {
    sprintf([]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: sprintf(): Argument #1 ($format) must be of type string, array given
