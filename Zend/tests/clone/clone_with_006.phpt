--TEST--
Clone with error cases
--FILE--
<?php

$x = new stdClass();

try {
	var_dump(clone($x, 1));
} catch (Throwable $e) {
	echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: Updated properties must be of type array, int given
