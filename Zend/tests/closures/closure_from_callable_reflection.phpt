--TEST--
Testing Closure::fromCallable() functionality: Reflection
--FILE--
<?php

class Bar {
	public static function staticMethod(Bar $bar, int $int, $none) {}
	public static function instanceMethod(Bar $bar, int $int, $none) {}
}

function foo(Bar $bar, int $int, $none) {

}

$fn = function (Bar $bar, int $x, $none) {};
$bar = new Bar();

$callables = [
	'foo',
	$fn,
	'Bar::staticMethod',
	[$bar, 'instanceMethod']
];


foreach ($callables as $callable) {
	$closure = Closure::fromCallable($callable);
	$refl = new ReflectionFunction($closure);
	foreach ($refl->getParameters() as $param) {
		if ($param->hasType()) {
			$type = $param->getType();
			echo $type->getName() . "\n";
		}
	}
}

?>
--EXPECTF--
Bar
int
Bar
int
Bar
int
Bar
int
