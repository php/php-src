--TEST--
SPL autoloader can be used to autoload dynamic functions
--FILE--
<?php
spl_autoload_register(function(string $name, int $mode) {
    eval("function $name() { echo 'name=$name, mode=$mode\n'; }");
}, true, false, SPL_AUTOLOAD_FUNCTION);

$func = 'foo1';
$func('bar');

call_user_func('foo2', 'baz');
call_user_func_array('foo3', ['baz']);

$reflect = new ReflectionFunction('foo4');
$reflect->invoke('baz');

eval("foo5('baz');");

(foo6(...))('bax');

$funcs = ['foo7', 'foo8'];

array_map(fn($f) => $f(), $funcs);
?>
--EXPECT--
name=foo1, mode=2
name=foo2, mode=2
name=foo3, mode=2
name=foo4, mode=2
name=foo5, mode=2
name=foo6, mode=2
name=foo7, mode=2
name=foo8, mode=2

