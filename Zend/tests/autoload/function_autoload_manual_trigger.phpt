--TEST--
Function autoloading: spl_autoload_call_function_loader() manual trigger
--FILE--
<?php
echo "== triggers and re-triggers ==\n";
$loader = function (string $name) {
    echo "loader($name)\n";
    // Probe with autoload disabled so the loader does not re-enter itself.
    if ($name === 'demo_func' && !function_exists('demo_func', false)) {
        eval('function demo_func() { return "manual"; }');
    }
};
spl_autoload_register_function_loader($loader);
var_dump(function_exists('demo_func', false)); // not consulted
spl_autoload_call_function_loader('demo_func'); // consulted, defines
var_dump(function_exists('demo_func'));          // already defined, not consulted
var_dump(demo_func());
spl_autoload_call_function_loader('demo_func'); // runs loaders again even though defined
spl_autoload_unregister_function_loader($loader);

echo "== non-existent stays undefined ==\n";
$loader = function (string $name) {
    echo "loader($name)\n";
};
spl_autoload_register_function_loader($loader);
spl_autoload_call_function_loader('missing_func');
echo "no exception\n";
var_dump(function_exists('missing_func', false));
spl_autoload_unregister_function_loader($loader);
?>
--EXPECT--
== triggers and re-triggers ==
bool(false)
loader(demo_func)
bool(true)
string(6) "manual"
loader(demo_func)
== non-existent stays undefined ==
loader(missing_func)
no exception
bool(false)
