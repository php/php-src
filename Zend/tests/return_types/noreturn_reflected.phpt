--TEST--
noreturn in reflection
--FILE--
<?php

function foo(): noreturn {}

$noreturnType = (new ReflectionFunction('foo'))->getReturnType();

echo $noreturnType->getName() . "\n";
var_dump($noreturnType->isBuiltin());

?>
--EXPECT--
noreturn
bool(true)