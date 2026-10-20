--TEST--
func_get_arg() invalid usage
--FILE--
<?php

try {
    var_dump(func_get_arg(1));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

function bar() {
    var_dump(func_get_arg(1));
}

function foo() {
    bar(func_get_arg(1));
}

try {
    foo(1,2);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: func_get_arg() cannot be called from the global scope
ValueError: func_get_arg(): Argument #1 ($position) must be less than the number of the arguments passed to the currently executed function
