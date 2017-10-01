--TEST--
Bug #74673 (Segfault when cast Reflection object to string with undefined constant)
--FILE--
<?php

set_error_handler(function() {
    throw new Exception();
});

class A
{
	public function method($test = PHP_SELF + 1)
	{
	}
}

$class = new ReflectionClass('A');

echo $class;
?>
--EXPECTF--
Fatal error: Method ReflectionClass::__toString() must not throw an exception, caught Exception:  in %sbug74673.php on line %d
