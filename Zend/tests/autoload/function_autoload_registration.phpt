--TEST--
Function autoloading: register, list, and unregister loaders
--FILE--
<?php
echo "== basic ==\n";
$basic = function (string $name) {
    echo "loader($name)\n";
    if ($name === 'demo_func') {
        eval('function demo_func() { return "autoloaded"; }');
    }
};
spl_autoload_register_function_loader($basic);
var_dump(demo_func());
var_dump(demo_func()); // already defined: loader not consulted again
spl_autoload_unregister_function_loader($basic);

echo "== list ==\n";
var_dump(spl_autoload_function_loaders());
$a = function (string $name) {};
$b = function (string $name) {};
spl_autoload_register_function_loader($a);
spl_autoload_register_function_loader($b);
$loaders = spl_autoload_function_loaders();
var_dump(count($loaders));
var_dump($loaders[0] === $a);
var_dump($loaders[1] === $b);
spl_autoload_unregister_function_loader($a);
spl_autoload_unregister_function_loader($b);

echo "== duplicate ==\n";
$dup = function (string $name) {};
spl_autoload_register_function_loader($dup);
spl_autoload_register_function_loader($dup);
var_dump(count(spl_autoload_function_loaders()));
spl_autoload_unregister_function_loader($dup);

echo "== unregister ==\n";
$reg = function (string $name) {
    eval("function $name() { return 'ok'; }");
};
spl_autoload_register_function_loader($reg);
var_dump(count(spl_autoload_function_loaders()));
spl_autoload_unregister_function_loader($reg);
var_dump(count(spl_autoload_function_loaders()));
try {
    missing_func();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

echo "== unregister false ==\n";
$once = function (string $name) {};
var_dump(spl_autoload_unregister_function_loader($once)); // never registered
spl_autoload_register_function_loader($once);
var_dump(spl_autoload_unregister_function_loader($once)); // removed
var_dump(spl_autoload_unregister_function_loader($once)); // already gone
?>
--EXPECT--
== basic ==
loader(demo_func)
string(10) "autoloaded"
string(10) "autoloaded"
== list ==
array(0) {
}
int(2)
bool(true)
bool(true)
== duplicate ==
int(1)
== unregister ==
int(1)
int(0)
Call to undefined function missing_func()
== unregister false ==
bool(false)
bool(true)
bool(false)
