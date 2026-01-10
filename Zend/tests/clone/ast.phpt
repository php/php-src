--TEST--
Ast Printing
--FILE--
<?php

$x = new stdClass();


try {
	assert(false &&  $y = clone $x);
} catch (Error $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	assert(false && $y = clone($x));
} catch (Error $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	assert(false && $y = clone($x, ));
} catch (Error $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	assert(false && $y = clone($x, [ "foo" => $foo, "bar" => $bar ]));
} catch (Error $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	assert(false && $y = clone($x, $array));
} catch (Error $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	assert(false && $y = clone($x, $array, $extraParameter, $trailingComma, ));
} catch (Error $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	assert(false && $y = clone(object: $x, withProperties: [ "foo" => $foo, "bar" => $bar ]));
} catch (Error $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	assert(false && $y = clone($x, withProperties: [ "foo" => $foo, "bar" => $bar ]));
} catch (Error $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	assert(false && $y = clone(object: $x));
} catch (Error $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	assert(false && $y = clone(object: $x, [ "foo" => $foo, "bar" => $bar ]));
} catch (Error $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	assert(false && $y = clone(...["object" => $x, "withProperties" => [ "foo" => $foo, "bar" => $bar ]]));
} catch (Error $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	assert(false && $y = clone(...));
} catch (Error $e) {
	echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
assert(false && ($y = \clone($x)))
assert(false && ($y = \clone($x)))
assert(false && ($y = \clone($x)))
assert(false && ($y = \clone($x, ['foo' => $foo, 'bar' => $bar])))
assert(false && ($y = \clone($x, $array)))
assert(false && ($y = \clone($x, $array, $extraParameter, $trailingComma)))
assert(false && ($y = \clone(object: $x, withProperties: ['foo' => $foo, 'bar' => $bar])))
assert(false && ($y = \clone($x, withProperties: ['foo' => $foo, 'bar' => $bar])))
assert(false && ($y = \clone(object: $x)))
assert(false && ($y = \clone(object: $x, ['foo' => $foo, 'bar' => $bar])))
assert(false && ($y = \clone(...['object' => $x, 'withProperties' => ['foo' => $foo, 'bar' => $bar]])))
assert(false && ($y = \clone(...)))
