--TEST--
Function autoloading: name normalization and invalid names
--FILE--
<?php
echo "== leading backslash (direct) ==\n";
// The loader is consulted with the leading backslash stripped.
$loader = function (string $name) {
    echo "loader($name)\n";
    if ($name === 'Ns\prefixed_func') {
        eval('namespace Ns; function prefixed_func() { return "ok"; }');
    }
};
spl_autoload_register_function_loader($loader);
var_dump(function_exists('\\Ns\\prefixed_func', true));
var_dump(\Ns\prefixed_func());
spl_autoload_unregister_function_loader($loader);

echo "== leading backslash (callable string) ==\n";
// The callable branch strips the leading backslash the same way.
$loader = function (string $name) {
    echo "loader($name)\n";
    if ($name === 'Ns\bs_func') {
        eval('namespace Ns; function bs_func() { return "ok"; }');
    }
};
spl_autoload_register_function_loader($loader);
var_dump(is_callable('\Ns\bs_func'));
var_dump(call_user_func('\Ns\bs_func'));
spl_autoload_unregister_function_loader($loader);

echo "== case insensitive ==\n";
// The loader receives the original case; a differently-cased call does not
// autoload again.
$loader = function (string $name) {
    echo "loader($name)\n";
    if (strtolower($name) === 'my_func') {
        eval('function my_func() { return "loaded"; }');
    }
};
spl_autoload_register_function_loader($loader);
var_dump(My_Func());
var_dump(MY_FUNC());
spl_autoload_unregister_function_loader($loader);

echo "== invalid names rejected ==\n";
// Invalid or empty names never reach the loader; a valid one still does.
$loader = function (string $name) {
    echo "loader($name)\n";
    if ($name === 'demo_func') {
        eval('function demo_func() { echo "demo_func called\n"; }');
    }
};
spl_autoload_register_function_loader($loader);
$f = 'foo bar';
try {
    $f();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump(function_exists('', true));
var_dump(function_exists('foo bar', true));
$g = 'demo_func';
$g();
spl_autoload_unregister_function_loader($loader);
?>
--EXPECT--
== leading backslash (direct) ==
loader(Ns\prefixed_func)
bool(true)
string(2) "ok"
== leading backslash (callable string) ==
loader(Ns\bs_func)
bool(true)
string(2) "ok"
== case insensitive ==
loader(My_Func)
string(6) "loaded"
string(6) "loaded"
== invalid names rejected ==
Call to undefined function foo bar()
bool(false)
bool(false)
loader(demo_func)
demo_func called
