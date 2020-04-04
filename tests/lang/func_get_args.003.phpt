--TEST--
func_get_args() outside of a function declaration
--FILE--
<?php

try {
    var_dump(func_get_args());
} catch (\Error $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
func_get_args() cannot be called from the global scope
