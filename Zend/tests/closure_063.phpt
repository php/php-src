--TEST--
Closure 063: Checking that closure is static
--FILE--
<?php

$staticUnscoped = static function () {};
$nonstaticUnscoped = function () {};

class A {
    function getStaticClosure() {
        return static function () {};
    }
    function getNonStaticClosure() {
        return function () {};
    }
}

$a = new A();
$staticScoped = $a->getStaticClosure();
$nonstaticScoped = $a->getNonStaticClosure();

function function_callable() {}

$nonstaticFromFunction = Closure::fromCallable('function_callable');

class B {
    function nonStaticMethod() {}
    static function staticMethod() {}
}

$staticFromMethod = Closure::fromCallable([B::class, 'staticMethod']);
$nonstaticFromMethod = Closure::fromCallable([new B(), 'nonStaticMethod']);

echo "Unscoped Closure\n";
var_dump($staticUnscoped->isStatic());
var_dump($nonstaticUnscoped->isStatic());
echo "\n";

echo "Scoped Closure\n";
var_dump($staticScoped->isStatic());
var_dump($nonstaticScoped->isStatic());
echo "\n";

echo "From Callable Function\n";
var_dump($nonstaticFromFunction->isStatic());
echo "\n";

echo "From Callable Method\n";
var_dump($staticFromMethod->isStatic());
var_dump($nonstaticFromMethod->isStatic());
echo "\n";

echo "Done\n";
?>
--EXPECTF--
Unscoped Closure
bool(true)
bool(false)

Scoped Closure
bool(true)
bool(false)

From Callable Function
bool(false)

From Callable Method
bool(true)
bool(false)

Done
