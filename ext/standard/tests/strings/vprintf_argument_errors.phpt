--TEST--
Test vprintf() - argument errors
--FILE--
<?php

try {
    vprintf('%s', 'value');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: vprintf(): Argument #2 ($values) must be of type array, string given
