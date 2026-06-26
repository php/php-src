--TEST--
Function autoloading: method and trampoline loaders
--FILE--
<?php
class StaticLoader {
    public static function load(string $name): void {
        if ($name === 'demo_func') {
            eval('function demo_func() { return "from_static"; }');
        }
    }
}

class InstanceLoader {
    public function load(string $name): void {
        if ($name === 'demo_func2') {
            eval('function demo_func2() { return "from_instance"; }');
        }
    }
}

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo "Trampoline for $name: $arguments[0]\n";
    }
}

echo "== static method ==\n";
spl_autoload_register_function_loader([StaticLoader::class, 'load']);
var_dump(demo_func());

echo "== instance method ==\n";
$obj = new InstanceLoader();
spl_autoload_register_function_loader([$obj, 'load']);
var_dump(demo_func2());
var_dump(count(spl_autoload_function_loaders()));
spl_autoload_unregister_function_loader([StaticLoader::class, 'load']);
spl_autoload_unregister_function_loader([$obj, 'load']);

echo "== trampoline ==\n";
$o = new TrampolineTest();
$callback1 = [$o, 'trampoline1'];
$callback2 = [$o, 'trampoline2'];
spl_autoload_register_function_loader($callback1);
spl_autoload_register_function_loader($callback2);
spl_autoload_register_function_loader($callback1); // duplicate, ignored
var_dump(count(spl_autoload_function_loaders()));
var_dump(function_exists('demo_func3', true)); // consults both trampolines, stays false
var_dump(spl_autoload_unregister_function_loader($callback1));
var_dump(spl_autoload_unregister_function_loader($callback1));
var_dump(spl_autoload_unregister_function_loader($callback2));
var_dump(spl_autoload_function_loaders());
var_dump(function_exists('demo_func3', true));
?>
--EXPECT--
== static method ==
string(11) "from_static"
== instance method ==
string(13) "from_instance"
int(2)
== trampoline ==
int(2)
Trampoline for trampoline1: demo_func3
Trampoline for trampoline2: demo_func3
bool(false)
bool(true)
bool(false)
bool(true)
array(0) {
}
bool(false)
