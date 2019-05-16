--TEST--
Bug #60367 (Reflection and Late Static Binding)
--FILE--
<?php
abstract class A {

	const WHAT = 'A';

	public static function call() {
		echo static::WHAT;
	}

}

class B extends A {

	const WHAT = 'B';

}

$method = new ReflectionMethod("b::call");
$method->invoke(null);
$method->invokeArgs(null, array());
$method = new ReflectionMethod("A::call");
$method->invoke(null);
$method->invokeArgs(null, array());
--EXPECT--
BBAA
