--TEST--
func_get_arg outside of a function declaration
--FILE--
<?php

try {
    var_dump(func_get_arg(0));
} catch (\Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: func_get_arg() cannot be called from the global scope
