--TEST--
spl_autoload_call_function_loader() manual trigger
--FILE--
<?php
$log = [];

spl_autoload_register_function_loader(function (string $name) use (&$log) {
    $log[] = $name;
    if ($name === 'manual_func' && !function_exists('manual_func')) {
        eval('function manual_func() { return "manual"; }');
    }
});

var_dump(function_exists('manual_func'));
spl_autoload_call_function_loader('manual_func');
var_dump(function_exists('manual_func'));
var_dump(manual_func());

// runs the loaders again even though manual_func() exists now
spl_autoload_call_function_loader('manual_func');
var_dump($log);
?>
--EXPECT--
bool(false)
bool(true)
string(6) "manual"
array(2) {
  [0]=>
  string(11) "manual_func"
  [1]=>
  string(11) "manual_func"
}
