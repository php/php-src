--TEST--
Function autoloading is case-insensitive
--FILE--
<?php
$log = [];

spl_autoload_register_function_loader(function (string $name) use (&$log) {
    $log[] = $name;
    if (strtolower($name) === 'my_func') {
        eval('function my_func() { return "loaded"; }');
    }
});

var_dump(My_Func());

var_dump(MY_FUNC()); // different case, no second autoload

var_dump($log);
?>
--EXPECT--
string(6) "loaded"
string(6) "loaded"
array(1) {
  [0]=>
  string(7) "My_Func"
}
