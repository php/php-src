--TEST--
Return type of self is allowed in closure but $this return value must be checked as closure might not be bound to a class
--FILE--
<?php

$c = function(): self { return $this; };
try {
	var_dump($c());
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Error: Using $this when not in object context
