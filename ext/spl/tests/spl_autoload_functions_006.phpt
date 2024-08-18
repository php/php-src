--TEST--
SPL autoloader can be used to autoload dynamic functions
--FILE--
<?php
spl_autoload_register(function(string $name, int $mode) {
    eval("function $name() { echo 'name=$name, mode=$mode\n'; }");
}, true, false, SPL_AUTOLOAD_FUNCTION);

$func = 'Foo1';
$func('bar');

call_user_func('Foo2', 'baz');
call_user_func_array('Foo3', ['baz']);

$reflect = new ReflectionFunction('Foo4');
$reflect->invoke('baz');

eval("Foo5('baz');");

(Foo6(...))('bax');

$funcs = ['Foo7', 'Foo8'];

array_map(fn($f) => $f(), $funcs);

Foo9();

?>
--EXPECT--
name=Foo1, mode=2
name=Foo2, mode=2
name=Foo3, mode=2
name=Foo4, mode=2
name=Foo5, mode=2
name=Foo6, mode=2
name=Foo7, mode=2
name=Foo8, mode=2
name=Foo9, mode=2
