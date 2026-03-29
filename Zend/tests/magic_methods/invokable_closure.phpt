--TEST--
Closure implements Invokable
--FILE--
<?php

/* Arrow function */
$fn = fn() => 1;
var_dump($fn instanceof Invokable);

/* Anonymous function */
$fn2 = function(int $x): int { return $x * 2; };
var_dump($fn2 instanceof Invokable);

/* Closure::fromCallable */
$fn3 = Closure::fromCallable('strlen');
var_dump($fn3 instanceof Invokable);

/* First-class callable syntax */
$fn4 = strlen(...);
var_dump($fn4 instanceof Invokable);

/* Reflection confirms Closure implements Invokable */
var_dump(in_array('Invokable', (new ReflectionClass(Closure::class))->getInterfaceNames()));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
