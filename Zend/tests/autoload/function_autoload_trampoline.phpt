--TEST--
Trampoline as function autoloader
--FILE--
<?php
class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo "Trampoline for $name: $arguments[0]\n";
    }
}
$o = new TrampolineTest();
$callback1 = [$o, 'trampoline1'];
$callback2 = [$o, 'trampoline2'];

spl_autoload_register_function_loader($callback1);
spl_autoload_register_function_loader($callback2);
spl_autoload_register_function_loader($callback1);

var_dump(count(spl_autoload_function_loaders()));
var_dump(function_exists('tramp_func', true));

var_dump(spl_autoload_unregister_function_loader($callback1));
var_dump(spl_autoload_unregister_function_loader($callback1));
var_dump(spl_autoload_unregister_function_loader($callback2));

var_dump(spl_autoload_function_loaders());
var_dump(function_exists('tramp_func', true));
?>
--EXPECT--
int(2)
Trampoline for trampoline1: tramp_func
Trampoline for trampoline2: tramp_func
bool(false)
bool(true)
bool(false)
bool(true)
array(0) {
}
bool(false)
