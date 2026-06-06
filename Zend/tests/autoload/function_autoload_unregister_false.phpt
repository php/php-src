--TEST--
Unregistering a function autoloader that was never registered returns false
--FILE--
<?php
$loader = function (string $name) {};

var_dump(spl_autoload_unregister_function_loader($loader));

spl_autoload_register_function_loader($loader);
var_dump(spl_autoload_unregister_function_loader($loader));
var_dump(spl_autoload_unregister_function_loader($loader));
?>
--EXPECT--
bool(false)
bool(true)
bool(false)
