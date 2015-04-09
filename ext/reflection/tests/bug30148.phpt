--TEST--
Reflection Bug #30148 (ReflectionMethod->isConstructor() fails for inherited classes)
--FILE--
<?php

class Root
{
	function Root() {}
}
class Base extends Root
{
	function __construct() {}
}
class Derived extends Base
{
}
$a = new ReflectionMethod('Root','Root');
$b = new ReflectionMethod('Base','Root');
$c = new ReflectionMethod('Base','__construct');
$d = new ReflectionMethod('Derived','Root');
$e = new ReflectionMethod('Derived','__construct');
var_dump($a->isConstructor());
var_dump($b->isConstructor());
var_dump($c->isConstructor());
var_dump($d->isConstructor());
var_dump($e->isConstructor());
?>
===DONE===
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
===DONE===
