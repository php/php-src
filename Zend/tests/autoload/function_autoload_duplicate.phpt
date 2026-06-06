--TEST--
Duplicate function autoloader registration is ignored
--FILE--
<?php
$loader = function (string $name) {};

spl_autoload_register_function_loader($loader);
spl_autoload_register_function_loader($loader);

var_dump(count(spl_autoload_function_loaders()));
?>
--EXPECT--
int(1)
