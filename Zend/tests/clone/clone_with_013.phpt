--TEST--
Clone with references
--FILE--
<?php

$x = new stdClass();

$ref = 'reference';
$with = ['x' => &$ref];

try {
	var_dump(clone($x, $with));
} catch (Throwable $e) {
	echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

unset($ref);

try {
	var_dump(clone($x, $with));
} catch (Throwable $e) {
	echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
Error: Cannot assign by reference when cloning with updated properties
object(stdClass)#%d (1) {
  ["x"]=>
  string(9) "reference"
}
