--TEST--
Function autoloader prepend
--FILE--
<?php
$log = [];

spl_autoload_register_function_loader(function (string $name) use (&$log) {
    $log[] = "first";
});

spl_autoload_register_function_loader(function (string $name) use (&$log) {
    $log[] = "prepended";
    if ($name === 'test_func') {
        eval('function test_func() { return "ok"; }');
    }
}, prepend: true);

var_dump(test_func());
var_dump($log);
?>
--EXPECT--
string(2) "ok"
array(1) {
  [0]=>
  string(9) "prepended"
}
