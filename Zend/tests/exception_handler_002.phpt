--TEST--
exception handler tests - 2
--FILE--
<?php

set_exception_handler("foo");

function foo($e) {
	var_dump(get_class($e)." thrown!");
	throw new Exception();
}

class test extends Exception {
}

throw new test();

echo "Done\n";
?>
--EXPECTF--	
string(12) "test thrown!"

Fatal error: Exception thrown without a stack frame in Unknown on line 0
