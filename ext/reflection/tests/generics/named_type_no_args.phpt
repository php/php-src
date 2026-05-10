--TEST--
Reflection: hasGenericArguments returns false when no arguments
--FILE--
<?php
function f(int $x): string { return ""; }
$r = new ReflectionFunction('f');
var_dump($r->getParameters()[0]->getType()->hasGenericArguments());
var_dump($r->getReturnType()->hasGenericArguments());
?>
--EXPECT--
bool(false)
bool(false)
