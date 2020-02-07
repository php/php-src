--TEST--
passing the return value from a function by reference
--FILE--
<?php

function foo(&$ref)
{
    var_dump($ref);
}

function bar($value)
{
    return $value;
}

try {
	foo(bar(5));
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}

?>
--EXPECT--
Exception: Cannot pass parameter 1 by reference
