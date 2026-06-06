--TEST--
Function autoloader unregister
--FILE--
<?php
$loader = function (string $name) {
    eval("function $name() { return 'ok'; }");
};

spl_autoload_register_function_loader($loader);
var_dump(count(spl_autoload_function_loaders()));

spl_autoload_unregister_function_loader($loader);
var_dump(count(spl_autoload_function_loaders()));

try {
    missing_func();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
int(1)
int(0)
Call to undefined function missing_func()
