--TEST--
Function autoloading with static method and instance method as autoloader
--FILE--
<?php
class StaticLoader {
    public static function load(string $name): void {
        if ($name === 'static_loaded') {
            eval('function static_loaded() { return "from_static"; }');
        }
    }
}

class InstanceLoader {
    public function load(string $name): void {
        if ($name === 'instance_loaded') {
            eval('function instance_loaded() { return "from_instance"; }');
        }
    }
}

spl_autoload_register_function_loader([StaticLoader::class, 'load']);
var_dump(static_loaded());

$obj = new InstanceLoader();
spl_autoload_register_function_loader([$obj, 'load']);
var_dump(instance_loaded());

var_dump(count(spl_autoload_function_loaders()));
?>
--EXPECT--
string(11) "from_static"
string(13) "from_instance"
int(2)
