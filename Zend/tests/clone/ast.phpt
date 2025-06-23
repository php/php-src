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

?>
--EXPECT--
assert(false && ($y = \clone($x)))
assert(false && ($y = \clone($x)))
