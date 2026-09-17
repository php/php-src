--TEST--
Ast Printing
--FILE--
<?php

$x = new stdClass();


try {
	assert(false &&  $y = clone $x);
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	assert(false && $y = clone($x));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	assert(false && $y = clone($x, ));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	assert(false && $y = clone($x, [ "foo" => $foo, "bar" => $bar ]));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	assert(false && $y = clone($x, $array));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	assert(false && $y = clone($x, $array, $extraParameter, $trailingComma, ));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	assert(false && $y = clone(object: $x, withProperties: [ "foo" => $foo, "bar" => $bar ]));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	assert(false && $y = clone($x, withProperties: [ "foo" => $foo, "bar" => $bar ]));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	assert(false && $y = clone(object: $x));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	assert(false && $y = clone(object: $x, [ "foo" => $foo, "bar" => $bar ]));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	assert(false && $y = clone(...["object" => $x, "withProperties" => [ "foo" => $foo, "bar" => $bar ]]));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	assert(false && $y = clone(...));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
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
