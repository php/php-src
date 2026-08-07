--TEST--
Testing func_get_args() throws error when called from the global scope
--FILE--
<?php

try {
    func_get_args();
} catch (\Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: func_get_args() cannot be called from the global scope
