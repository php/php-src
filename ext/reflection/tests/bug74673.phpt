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
Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 [internal function]: {closure}(2, 'Use of undefine...', %s, %d, Array)
#1 %s(%d): ReflectionClass->__toString()
#2 {main}
  thrown in %s on line %d
