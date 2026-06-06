--TEST--
Function autoloading strips leading backslash from name passed to autoloader
--FILE--
<?php
$log = [];

spl_autoload_register_function_loader(function (string $name) use (&$log) {
    $log[] = $name;
    if ($name === 'Ns\prefixed_func') {
        eval('namespace Ns; function prefixed_func() { return "ok"; }');
    }
});

var_dump(function_exists('\\Ns\\prefixed_func', true));
var_dump(\Ns\prefixed_func());
var_dump($log);
?>
--EXPECT--
bool(true)
string(2) "ok"
array(1) {
  [0]=>
  string(16) "Ns\prefixed_func"
}
