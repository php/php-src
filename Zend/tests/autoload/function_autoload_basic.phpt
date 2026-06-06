--TEST--
Basic function autoloading
--FILE--
<?php
spl_autoload_register_function_loader(function (string $name) {
    echo "autoloading -- ";
    if ($name === 'test_func') {
        eval('function test_func() { return "autoloaded"; }');
    }
});

var_dump(test_func());
var_dump(test_func());
?>
--EXPECT--
autoloading -- string(10) "autoloaded"
string(10) "autoloaded"
