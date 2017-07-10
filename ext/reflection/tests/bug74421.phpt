--TEST--
Bug #74421 Wrong reflection on ReflectionFunction::invoke
--FILE--
<?php
$rf = new ReflectionMethod(ReflectionFunction::class, 'invoke');
var_dump($rf->isVariadic());
var_dump($rf->getParameters()[0]->isVariadic());

$rm = new ReflectionMethod(ReflectionMethod::class, 'invoke');
var_dump($rm->isVariadic());
var_dump($rm->getParameters()[1]->isVariadic());
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
