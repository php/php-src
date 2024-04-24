--TEST--
Check that a bound Closure retrieved via getDeclaringFunction() can still resolve types
--FILE--
<?php

class A {}
class B extends A {}
class C extends B {}

$closureSelf = function($op1, $op2 = 0): self { };
$closureParent = function($op1, $op2 = 0): parent { };
$closureStatic = function($op1, $op2 = 0): static { };

const CLOSURE_SELF_NAME = '{closure:' . __FILE__ . ':7}';
const CLOSURE_PARENT_NAME = '{closure:' . __FILE__ . ':8}';
const CLOSURE_STATIC_NAME = '{closure:' . __FILE__ . ':9}';

$cases = [
    [$closureSelf, CLOSURE_SELF_NAME],
    [$closureParent, CLOSURE_PARENT_NAME],
    [$closureStatic, CLOSURE_STATIC_NAME],
];

$c = new C();
foreach ($cases as [$unboundFn, $name]) {
    $fn = $unboundFn->bindTo($c, B::class);
    $rClosure = new ReflectionFunction($fn);
    var_dump($rClosure->name == $name);

    $params = $rClosure->getParameters();
    unset ($rClosure);

    $closureFromParam = $params[0]->getDeclaringFunction();
    var_dump($closureFromParam->name == $name);
    var_dump($closureFromParam::class);
    $type = $closureFromParam->getReturnType();
    var_dump((string) $type);
    $resolvedType = $type->resolveToNamedType();
    var_dump((string) $resolvedType);
}

?>
--EXPECT--
bool(true)
bool(true)
string(16) "ReflectionMethod"
string(4) "self"
string(1) "B"
bool(true)
bool(true)
string(16) "ReflectionMethod"
string(6) "parent"
string(1) "A"
bool(true)
bool(true)
string(16) "ReflectionMethod"
string(6) "static"
string(1) "C"
