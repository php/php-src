--TEST--
ReflectionFunction::hasParameter()
--FILE--
<?php
function foo(string $bar) {}

$function = new ReflectionFunction('sort');
var_dump($function->hasParameter('array'));
var_dump($function->hasParameter('Array'));
var_dump($function->hasParameter('string'));
var_dump($function->hasParameter(1));
var_dump($function->hasParameter(2));

$function = new ReflectionFunction('foo');
var_dump($function->hasParameter('bar'));
var_dump($function->hasParameter('Bar'));
var_dump($function->hasParameter('string'));
var_dump($function->hasParameter(1));
var_dump($function->hasParameter(2));
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
