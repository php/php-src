--TEST--
Multiple function autoloaders
--FILE--
<?php
$log = [];

spl_autoload_register_function_loader(function (string $name) use (&$log) {
    $log[] = "loader1: $name";
});

spl_autoload_register_function_loader(function (string $name) use (&$log) {
    $log[] = "loader2: $name";
    if ($name === 'test_func') {
        eval('function test_func() { return "from_loader2"; }');
    }
});

// never called, loader2 already defined test_func
spl_autoload_register_function_loader(function (string $name) use (&$log) {
    $log[] = "loader3: $name";
});

var_dump(test_func());
var_dump($log);
?>
--EXPECT--
string(12) "from_loader2"
array(2) {
  [0]=>
  string(18) "loader1: test_func"
  [1]=>
  string(18) "loader2: test_func"
}
