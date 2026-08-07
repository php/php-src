--TEST--
Ast Printing
--FILE--
<?php

$x = new stdClass();


try {
	assert(false &&  $y = clone $x);
} catch (Error $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	assert(false && $y = clone($x));
} catch (Error $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	assert(false && $y = clone($x, ));
} catch (Error $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	assert(false && $y = clone($x, [ "foo" => $foo, "bar" => $bar ]));
} catch (Error $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	assert(false && $y = clone($x, $array));
} catch (Error $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	assert(false && $y = clone($x, $array, $extraParameter, $trailingComma, ));
} catch (Error $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	assert(false && $y = clone(object: $x, withProperties: [ "foo" => $foo, "bar" => $bar ]));
} catch (Error $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	assert(false && $y = clone($x, withProperties: [ "foo" => $foo, "bar" => $bar ]));
} catch (Error $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	assert(false && $y = clone(object: $x));
} catch (Error $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	assert(false && $y = clone(object: $x, [ "foo" => $foo, "bar" => $bar ]));
} catch (Error $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	assert(false && $y = clone(...["object" => $x, "withProperties" => [ "foo" => $foo, "bar" => $bar ]]));
} catch (Error $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	assert(false && $y = clone(...));
} catch (Error $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
AssertionError: assert(false && ($y = \clone($x)))
AssertionError: assert(false && ($y = \clone($x)))
AssertionError: assert(false && ($y = \clone($x)))
AssertionError: assert(false && ($y = \clone($x, ['foo' => $foo, 'bar' => $bar])))
AssertionError: assert(false && ($y = \clone($x, $array)))
AssertionError: assert(false && ($y = \clone($x, $array, $extraParameter, $trailingComma)))
AssertionError: assert(false && ($y = \clone(object: $x, withProperties: ['foo' => $foo, 'bar' => $bar])))
AssertionError: assert(false && ($y = \clone($x, withProperties: ['foo' => $foo, 'bar' => $bar])))
AssertionError: assert(false && ($y = \clone(object: $x)))
AssertionError: assert(false && ($y = \clone(object: $x, ['foo' => $foo, 'bar' => $bar])))
AssertionError: assert(false && ($y = \clone(...['object' => $x, 'withProperties' => ['foo' => $foo, 'bar' => $bar]])))
AssertionError: assert(false && ($y = \clone(...)))
