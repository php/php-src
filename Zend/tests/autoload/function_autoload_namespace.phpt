--TEST--
Function autoloading: namespaced names and global fallback
--FILE--
<?php
namespace {
    echo "== fully qualified ==\n";
    $loader = function (string $name) {
        echo "loader($name)\n";
        if ($name === 'App\Util\helper') {
            eval('namespace App\Util; function helper() { return "ns_helper"; }');
        }
    };
    spl_autoload_register_function_loader($loader);
    var_dump(\App\Util\helper());
    spl_autoload_unregister_function_loader($loader);
}

namespace App {
    echo "== global fallback ==\n";
    // local_func resolves to App\local_func (loader defines it namespaced).
    // global_func is answered with a GLOBAL definition instead of App\global_func:
    // the current call still throws (an answer to a different question is not
    // rebound), then the next call finds the global through the normal fallback.
    $loader = function (string $name) {
        echo "loader($name)\n";
        if ($name === 'App\local_func') {
            eval('namespace App; function local_func() { return "local"; }');
        } elseif ($name === 'App\global_func') {
            eval('function global_func() { return "global"; }');
        }
    };
    \spl_autoload_register_function_loader($loader);
    var_dump(local_func());
    var_dump(local_func()); // already defined: loader not consulted again
    try {
        global_func();
    } catch (\Error $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump(global_func()); // global found via fallback: no re-consult
    \spl_autoload_unregister_function_loader($loader);
}

namespace App {
    echo "== namespaced loader exception ==\n";
    // The unqualified call resolves to App\some_func; the loader's exception
    // carries the namespaced name.
    $loader = function (string $name) {
        throw new \RuntimeException("Autoload failed for: $name");
    };
    \spl_autoload_register_function_loader($loader);
    try {
        some_func();
    } catch (\RuntimeException $e) {
        echo $e->getMessage(), "\n";
    }
    \spl_autoload_unregister_function_loader($loader);
}
?>
--EXPECT--
== fully qualified ==
loader(App\Util\helper)
string(9) "ns_helper"
== global fallback ==
loader(App\local_func)
string(5) "local"
string(5) "local"
loader(App\global_func)
Call to undefined function App\global_func()
string(6) "global"
== namespaced loader exception ==
Autoload failed for: App\some_func
