--TEST--
SPL autoloader can be used to autoload dynamic functions
--FILE--
<?php
spl_autoload_register(function(string $name) {
    eval("function $name() { echo 'name=$name\n'; }");
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
name=Foo1
name=Foo2
name=Foo3
name=Foo4
name=Foo5
name=Foo6
name=Foo7
name=Foo8
name=Foo9
