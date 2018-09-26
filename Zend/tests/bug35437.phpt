--TEST--
Bug #35437 (Segfault or Invalid Opcode 137/1/4)
--FILE--
<?php
function err2exception($errno, $errstr)
{
	throw new Exception("Error occuried: " . $errstr);
}

set_error_handler('err2exception');

class TestClass
{
	function testMethod()
	{
		$GLOBALS['t'] = new stdClass;
	}
}

try {
	TestClass::testMethod();
} catch (Exception $e) {
	echo "Caught: ".$e->getMessage()."\n";
}
?>
--EXPECT--
Caught: Error occuried: Non-static method TestClass::testMethod() should not be called statically
