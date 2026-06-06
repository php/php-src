--TEST--
spl_autoload_function_loaders() lists registered loaders
--FILE--
<?php
var_dump(spl_autoload_function_loaders());

$a = function (string $name) {};
$b = function (string $name) {};

spl_autoload_register_function_loader($a);
spl_autoload_register_function_loader($b);

$loaders = spl_autoload_function_loaders();
var_dump(count($loaders));
var_dump($loaders[0] === $a);
var_dump($loaders[1] === $b);
?>
--EXPECT--
array(0) {
}
int(2)
bool(true)
bool(true)
