--TEST--
Function autoloader unregistering itself during autoloading skips the next loader
--FILE--
<?php
$log = [];

$first = function (string $name) use (&$log, &$first) {
    $log[] = "first: $name";
    spl_autoload_unregister_function_loader($first);
};

$second = function (string $name) use (&$log) {
    $log[] = "second: $name";
    if ($name === 'test_func') {
        eval('function test_func() { return "ok"; }');
    }
};

spl_autoload_register_function_loader($first);
spl_autoload_register_function_loader($second);

try {
    test_func();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump(count(spl_autoload_function_loaders()));
var_dump($log);

var_dump(test_func());
var_dump($log);
?>
--EXPECT--
Call to undefined function test_func()
int(1)
array(1) {
  [0]=>
  string(16) "first: test_func"
}
string(2) "ok"
array(2) {
  [0]=>
  string(16) "first: test_func"
  [1]=>
  string(17) "second: test_func"
}
