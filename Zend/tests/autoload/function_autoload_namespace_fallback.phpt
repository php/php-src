--TEST--
Namespace fallback function autoloading
--FILE--
<?php
namespace App;

\spl_autoload_register_function_loader(function (string $name) {
    echo "Autoloading: $name\n";
    if ($name === 'App\local_func') {
        eval('namespace App; function local_func() { return "local"; }');
    } elseif ($name === 'App\global_func') {
        eval('function global_func() { return "global"; }');
    }
});

var_dump(local_func());
var_dump(local_func());

try {
    global_func();
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump(global_func());
?>
--EXPECT--
Autoloading: App\local_func
string(5) "local"
string(5) "local"
Autoloading: App\global_func
Call to undefined function App\global_func()
string(6) "global"
