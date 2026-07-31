--TEST--
Function autoloading: recursion guard and loader-list mutation
--FILE--
<?php
echo "== recursion prevented ==\n";
// Calling the very function being autoloaded does not re-enter the loader for
// that name; the inner call throws, and so does the outer one.
$loader = function (string $name) {
    echo "loader($name)\n";
    if ($name === 'recursive_func') {
        try {
            recursive_func();
        } catch (Error $e) {
            echo "Caught: " . $e->getMessage() . "\n";
        }
    }
};
spl_autoload_register_function_loader($loader);
try {
    recursive_func();
} catch (Error $e) {
    echo "Final: " . $e->getMessage() . "\n";
}
spl_autoload_unregister_function_loader($loader);

echo "== register during autoload ==\n";
// A loader that registers another loader mid-pass: the newly added loader is
// consulted in the same pass and defines the function.
$second = function (string $name) {
    echo "second: $name\n";
    if ($name === 'demo_func') {
        eval('function demo_func() { return "ok"; }');
    }
};
$first = function (string $name) use (&$second) {
    echo "first: $name\n";
    spl_autoload_register_function_loader($second);
};
spl_autoload_register_function_loader($first);
var_dump(demo_func());
spl_autoload_unregister_function_loader($first);
spl_autoload_unregister_function_loader($second);

echo "== unregister during autoload ==\n";
// A loader that unregisters itself mid-pass skips the next loader on this pass;
// a later call reaches the surviving loader and succeeds.
$second = function (string $name) {
    echo "second: $name\n";
    if ($name === 'demo_func2') {
        eval('function demo_func2() { return "ok"; }');
    }
};
$first = function (string $name) use (&$first) {
    echo "first: $name\n";
    spl_autoload_unregister_function_loader($first);
};
spl_autoload_register_function_loader($first);
spl_autoload_register_function_loader($second);
try {
    demo_func2();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump(count(spl_autoload_function_loaders()));
var_dump(demo_func2());
spl_autoload_unregister_function_loader($second);
?>
--EXPECT--
== recursion prevented ==
loader(recursive_func)
Caught: Call to undefined function recursive_func()
Final: Call to undefined function recursive_func()
== register during autoload ==
first: demo_func
second: demo_func
string(2) "ok"
== unregister during autoload ==
first: demo_func2
Call to undefined function demo_func2()
int(1)
second: demo_func2
string(2) "ok"
