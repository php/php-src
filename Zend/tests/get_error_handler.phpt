--TEST--
get_error_handler()
--FILE--
<?php

class C {
    function handle() {}
    static function handleStatic() {}
}

class Invokable {
    public function __invoke() {
    }
}

function foo() {}

echo "No error handler\n";
var_dump(get_error_handler() === null);

echo "\nFunction string\n";
set_error_handler('foo');
var_dump(get_error_handler() === 'foo');

echo "\nNULL\n";
set_error_handler(null);
var_dump(get_error_handler() === null);

echo "\nStatic method array\n";
set_error_handler([C::class, 'handleStatic']);
var_dump(get_error_handler() === [C::class, 'handleStatic']);

echo "\nStatic method string\n";
set_error_handler('C::handleStatic');
var_dump(get_error_handler() === 'C::handleStatic');

echo "\nInstance method array\n";
set_error_handler([$c = new C(), 'handle']);
var_dump(get_error_handler() === [$c, 'handle']);

echo "\nFirst class callable method\n";
set_error_handler($f = (new C())->handle(...));
var_dump(get_error_handler() === $f);

echo "\nClosure\n";
set_error_handler($f = function () {});
var_dump(get_error_handler() === $f);

echo "\nInvokable\n";
set_error_handler($object = new Invokable());
var_dump(get_error_handler() === $object);

echo "\nStable return value\n";
var_dump(get_error_handler() === get_error_handler());

?>
==DONE==
--EXPECT--
No error handler
bool(true)

Function string
bool(true)

NULL
bool(true)

Static method array
bool(true)

Static method string
bool(true)

Instance method array
bool(true)

First class callable method
bool(true)

Closure
bool(true)

Invokable
bool(true)

Stable return value
bool(true)
==DONE==
