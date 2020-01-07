--TEST--
ReflectionType for static types
--FILE--
<?php

class A {
    public function test(): static {
        return new static;
    }
}

$rm = new ReflectionMethod(A::class, 'test');
$rt = $rm->getReturnType();
// TODO: Should it be considered a builtin or not?
var_dump($rt->isBuiltin());
var_dump($rt->getName());
var_dump((string) $rt);

?>
--EXPECT--
bool(true)
string(6) "static"
string(6) "static"
