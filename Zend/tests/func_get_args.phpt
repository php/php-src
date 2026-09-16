--TEST--
Testing func_get_args() throws error when called from the global scope
--FILE--
<?php

try {
    func_get_args();
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: func_get_args() cannot be called from the global scope
