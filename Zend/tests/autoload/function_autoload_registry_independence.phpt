--TEST--
Function autoloading: function and class loader registries are independent
--FILE--
<?php
class SymfonyShapedLoader {
    public static function load(string $name, ?\Exception $unexpected = null): void {
        if (func_num_args() > 1) {
            throw new \TypeError("class autoloader received an unexpected extra argument");
        }
        echo "class loader: $name\n";
    }
}

$classLoader = function (string $name) {
    echo "class loader: $name\n";
};
$funcLoader = function (string $name) {
    echo "function loader: $name\n";
};
spl_autoload_register($classLoader);
spl_autoload_register_function_loader($funcLoader);

echo "== invocation independence ==\n";
// A class lookup consults only class loaders; a function lookup only function
// loaders.
class_exists('SomeClass');
function_exists('some_func', true);

echo "== management API separation ==\n";
// Function and class autoloaders live in fully separate registries.
// Stas Malyshev's 2013 objection to a unified mechanism was that one callback
// list mixing different logic needs an ugly type switch. Each introspection,
// unregister, and manual-trigger entry point sees only its own registry.
var_dump(count(spl_autoload_functions()));
var_dump(count(spl_autoload_function_loaders()));
var_dump(in_array($classLoader, spl_autoload_functions(), true));
var_dump(in_array($funcLoader, spl_autoload_function_loaders(), true));
// A loader cannot be removed through the other registry's unregister function.
var_dump(spl_autoload_unregister($funcLoader));
var_dump(spl_autoload_unregister_function_loader($classLoader));
var_dump(count(spl_autoload_functions()));
var_dump(count(spl_autoload_function_loaders()));
// Manual triggering stays within its own registry.
spl_autoload_call('X');
spl_autoload_call_function_loader('y');
spl_autoload_unregister($classLoader);
spl_autoload_unregister_function_loader($funcLoader);

echo "== class loader arg arity unchanged ==\n";
// A class autoloader's argument arity is unchanged. The 2024
// function_autoloading4 RFC passed a $type argument to existing class
// autoloaders, crashing Symfony's loader; a separate registry touches nothing,
// so a class autoloader still receives exactly one argument.
spl_autoload_register([SymfonyShapedLoader::class, 'load']);
spl_autoload_register_function_loader(function (string $name) {
    if (func_num_args() > 1) {
        throw new \TypeError("function loader received an unexpected extra argument");
    }
    echo "function loader: $name\n";
});
class_exists('Some\Missing\ClassName');
function_exists('some_missing_function', true);
echo "done\n";
?>
--EXPECT--
== invocation independence ==
class loader: SomeClass
function loader: some_func
== management API separation ==
int(1)
int(1)
bool(true)
bool(true)
bool(false)
bool(false)
int(1)
int(1)
class loader: X
function loader: y
== class loader arg arity unchanged ==
class loader: Some\Missing\ClassName
function loader: some_missing_function
done
