--TEST--
function_exists() with autoload parameter
--FILE--
<?php
spl_autoload_register_function_loader(function (string $name) {
    if ($name === 'lazy_func') {
        eval('function lazy_func() { return "lazy"; }');
    }
});

var_dump(function_exists('lazy_func'));
var_dump(function_exists('lazy_func', true));
var_dump(function_exists('lazy_func'));
?>
--EXPECT--
bool(false)
bool(true)
bool(true)
