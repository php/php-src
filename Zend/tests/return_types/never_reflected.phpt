--TEST--
never in reflection
--FILE--
<?php

function foo(): never {}

$neverType = (new ReflectionFunction('foo'))->getReturnType();

echo $neverType->getName() . "\n";
var_dump($neverType->isBuiltin());

?>
--EXPECT--
never
bool(true)
