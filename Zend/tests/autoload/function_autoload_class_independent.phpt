--TEST--
Class and function autoloaders are independent
--FILE--
<?php
$class_log = [];
$func_log = [];

spl_autoload_register(function (string $name) use (&$class_log) {
    $class_log[] = $name;
});

spl_autoload_register_function_loader(function (string $name) use (&$func_log) {
    $func_log[] = $name;
});

class_exists('SomeClass');
function_exists('some_func', true);

echo "Class autoloader received:\n";
var_dump($class_log);
echo "Function autoloader received:\n";
var_dump($func_log);
?>
--EXPECT--
Class autoloader received:
array(1) {
  [0]=>
  string(9) "SomeClass"
}
Function autoloader received:
array(1) {
  [0]=>
  string(9) "some_func"
}
