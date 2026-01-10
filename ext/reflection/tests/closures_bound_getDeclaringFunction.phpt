--TEST--
Closures via getParameters() and getDeclaringFunction() calling reflection_method_factory() with an internal object being set
--FILE--
<?php

class A {}
class B extends A {}

$closure = function($op1, $op2 = 0): self { };

$b = new B();
$fn = $closure->bindTo($b, B::class);

const CLOSURE_NAME = '{closure:' . __FILE__ . ':6}';

$rClosure = new ReflectionFunction($fn);
var_dump($rClosure->name == CLOSURE_NAME);

$params = $rClosure->getParameters();
unset ($rClosure);
$closureFromParam = $params[0]->getDeclaringFunction();
var_dump($closureFromParam::class);
var_dump($closureFromParam->name == CLOSURE_NAME);

$rClass = new ReflectionClass($b);
$rMethods = $rClass->getMethods();
var_dump($rMethods);

try {
    $rMethod = new ReflectionMethod($b, CLOSURE_NAME);
    var_dump($rMethod);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
bool(true)
string(16) "ReflectionMethod"
bool(true)
array(0) {
}
ReflectionException: Method B::{closure:%s:6}() does not exist
