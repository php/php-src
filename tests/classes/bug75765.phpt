--TEST--
Ensure undefined class extending throws the exception.
--FILE--
<?php
try {
	class C extends UndefinedClass
	{
	}
} catch (Error $e) {
	var_dump($e->getMessage());
}

try {
	class C implements UndefinedInterface
	{
	}
} catch (Error $e) {
	var_dump($e->getMessage());
}
?>
--EXPECT--
string(32) "Class 'UndefinedClass' not found"
string(40) "Interface 'UndefinedInterface' not found"
