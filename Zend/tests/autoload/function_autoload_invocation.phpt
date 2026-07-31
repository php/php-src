--TEST--
Function autoloading: the call surfaces that trigger a loader
--FILE--
<?php
echo "== function_exists ==\n";
// $autoload defaults to true (matching class_exists()): the loader runs and
// defines the function. Explicit false probes the table without autoloading.
$loader = function (string $name) {
    echo "loader($name)\n";
    if ($name === 'demo_func') {
        eval('function demo_func() {}');
    }
};
spl_autoload_register_function_loader($loader);
var_dump(function_exists('demo_func'));        // default true: loader runs, defined
var_dump(function_exists('demo_func', false));  // already defined, no consult
var_dump(function_exists('demo_func', true));   // already defined, no consult
var_dump(function_exists('missing_func', false)); // explicit false: not consulted
var_dump(function_exists('missing_func'));      // default true: consulted, still false
spl_autoload_unregister_function_loader($loader);

echo "== is_callable ==\n";
$loader = function (string $name) {
    echo "loader($name)\n";
    if ($name === 'demo_func2') {
        eval('function demo_func2() {}');
    }
};
spl_autoload_register_function_loader($loader);
var_dump(is_callable('demo_func2'));        // full check autoloads
var_dump(is_callable('demo_func2'));        // already defined, no consult
var_dump(is_callable('syntax_only', true)); // syntax-only must not autoload
var_dump(is_callable('missing_func'));      // full check consults, stays false
spl_autoload_unregister_function_loader($loader);

echo "== call_user_func ==\n";
$loader = function (string $name) {
    echo "loader($name)\n";
    if ($name === 'demo_func3') {
        eval('function demo_func3($x) { return "cuf:$x"; }');
    } elseif ($name === 'demo_func4') {
        eval('function demo_func4($a, $b) { return "cufa:$a:$b"; }');
    }
};
spl_autoload_register_function_loader($loader);
var_dump(call_user_func('demo_func3', 1));
var_dump(call_user_func_array('demo_func4', [2, 3]));
spl_autoload_unregister_function_loader($loader);

echo "== array callbacks ==\n";
$loader = function (string $name) {
    echo "loader($name)\n";
    if ($name === 'doubler') {
        eval('function doubler($x) { return $x * 2; }');
    } elseif ($name === 'odd_filter') {
        eval('function odd_filter($x) { return $x % 2 === 1; }');
    } elseif ($name === 'comparator') {
        eval('function comparator($a, $b) { return $a <=> $b; }');
    }
};
spl_autoload_register_function_loader($loader);
echo implode(',', array_map('doubler', [1, 2, 3])), "\n";
echo implode(',', array_filter([1, 2, 3, 4], 'odd_filter')), "\n";
$data = [3, 1, 2];
usort($data, 'comparator');
echo implode(',', $data), "\n";
spl_autoload_unregister_function_loader($loader);

echo "== callable param coercion ==\n";
$loader = function (string $name) {
    echo "loader($name)\n";
    if ($name === 'demo_func5') {
        eval('function demo_func5($x) { return "cp:$x"; }');
    }
};
spl_autoload_register_function_loader($loader);
function apply(callable $fn, $arg) {
    return $fn($arg);
}
var_dump(apply('demo_func5', 5)); // type check resolves the string, autoloads
try {
    apply('missing_func', 0); // loader runs during the check, then TypeError
} catch (\TypeError $e) {
    echo get_class($e), "\n";
}
spl_autoload_unregister_function_loader($loader);

echo "== Closure::fromCallable ==\n";
// For a name that never resolves, fromCallable() may consult the loader more
// than once, so assert only that it ran (a flag), not an exact echo count.
$consulted = false;
$loader = function (string $name) use (&$consulted) {
    if ($name === 'missing_func') {
        $consulted = true;
        return;
    }
    echo "loader($name)\n";
    if ($name === 'demo_func6') {
        eval('function demo_func6($x) { return "fc:$x"; }');
    }
};
spl_autoload_register_function_loader($loader);
$closure = Closure::fromCallable('demo_func6');
var_dump($closure(7));
try {
    Closure::fromCallable('missing_func');
} catch (\TypeError $e) {
    echo get_class($e), "\n";
}
var_dump($consulted);
spl_autoload_unregister_function_loader($loader);

echo "== dynamic call ==\n";
$loader = function (string $name) {
    echo "loader($name)\n";
    if ($name === 'demo_func7') {
        eval('function demo_func7() { return "dynamic"; }');
    }
};
spl_autoload_register_function_loader($loader);
$f = 'demo_func7';
var_dump($f());
spl_autoload_unregister_function_loader($loader);

echo "== ReflectionFunction ==\n";
$loader = function (string $name) {
    echo "loader($name)\n";
    if ($name === 'demo_func8') {
        eval('function demo_func8() {}');
    }
};
spl_autoload_register_function_loader($loader);
$rf = new ReflectionFunction('demo_func8');
var_dump($rf->getName());
try {
    new ReflectionFunction('missing_func'); // loader consulted, then ReflectionException
} catch (\ReflectionException $e) {
    echo $e->getMessage(), "\n";
}
spl_autoload_unregister_function_loader($loader);
?>
--EXPECT--
== function_exists ==
loader(demo_func)
bool(true)
bool(true)
bool(true)
bool(false)
loader(missing_func)
bool(false)
== is_callable ==
loader(demo_func2)
bool(true)
bool(true)
bool(true)
loader(missing_func)
bool(false)
== call_user_func ==
loader(demo_func3)
string(5) "cuf:1"
loader(demo_func4)
string(8) "cufa:2:3"
== array callbacks ==
loader(doubler)
2,4,6
loader(odd_filter)
1,3
loader(comparator)
1,2,3
== callable param coercion ==
loader(demo_func5)
string(4) "cp:5"
loader(missing_func)
TypeError
== Closure::fromCallable ==
loader(demo_func6)
string(4) "fc:7"
TypeError
bool(true)
== dynamic call ==
loader(demo_func7)
string(7) "dynamic"
== ReflectionFunction ==
loader(demo_func8)
string(10) "demo_func8"
loader(missing_func)
Function missing_func() does not exist
