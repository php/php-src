--TEST--
Function autoloading: multiple loaders and prepend order
--FILE--
<?php
echo "== order ==\n";
$l1 = function (string $name) {
    echo "loader1: $name\n";
};
$l2 = function (string $name) {
    echo "loader2: $name\n";
    if ($name === 'demo_func') {
        eval('function demo_func() { return "from_loader2"; }');
    }
};
$l3 = function (string $name) {
    echo "loader3: $name\n"; // never reached: loader2 already defined it
};
spl_autoload_register_function_loader($l1);
spl_autoload_register_function_loader($l2);
spl_autoload_register_function_loader($l3);
var_dump(demo_func());
spl_autoload_unregister_function_loader($l1);
spl_autoload_unregister_function_loader($l2);
spl_autoload_unregister_function_loader($l3);

echo "== prepend ==\n";
$append = function (string $name) {
    echo "appended: $name\n"; // never reached: prepended runs first and defines
};
$prepend = function (string $name) {
    echo "prepended: $name\n";
    if ($name === 'demo_func2') {
        eval('function demo_func2() { return "ok"; }');
    }
};
spl_autoload_register_function_loader($append);
spl_autoload_register_function_loader($prepend, prepend: true);
var_dump(demo_func2());
spl_autoload_unregister_function_loader($append);
spl_autoload_unregister_function_loader($prepend);
?>
--EXPECT--
== order ==
loader1: demo_func
loader2: demo_func
string(12) "from_loader2"
== prepend ==
prepended: demo_func2
string(2) "ok"
