--TEST--
Function autoloading via dynamic call
--FILE--
<?php
spl_autoload_register_function_loader(function (string $name) {
    if ($name === 'dynamic_func') {
        eval('function dynamic_func() { return "dynamic"; }');
    }
});

$f = 'dynamic_func';
var_dump($f());
?>
--EXPECT--
string(7) "dynamic"
