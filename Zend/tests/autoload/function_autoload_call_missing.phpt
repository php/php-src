--TEST--
spl_autoload_call_function_loader() with non-existent function
--FILE--
<?php
$log = [];

spl_autoload_register_function_loader(function (string $name) use (&$log) {
    $log[] = $name;
});

spl_autoload_call_function_loader('nonexistent_func');
echo "No exception\n";
var_dump(function_exists('nonexistent_func'));
var_dump($log);
?>
--EXPECT--
No exception
bool(false)
array(1) {
  [0]=>
  string(16) "nonexistent_func"
}
