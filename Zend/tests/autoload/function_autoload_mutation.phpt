--TEST--
Function autoloader modifying autoloader list during autoloading
--FILE--
<?php
$log = [];

$first = function (string $name) use (&$log, &$second) {
    $log[] = "first: $name";
    spl_autoload_register_function_loader($second);
};

$second = function (string $name) use (&$log) {
    $log[] = "second: $name";
    if ($name === 'test_func') {
        eval('function test_func() { return "ok"; }');
    }
};

spl_autoload_register_function_loader($first);

var_dump(test_func());
var_dump($log);
?>
--EXPECT--
string(2) "ok"
array(2) {
  [0]=>
  string(16) "first: test_func"
  [1]=>
  string(17) "second: test_func"
}
