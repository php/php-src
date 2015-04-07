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

Exception: (empty message) in %sexception_handler_002.php on line 7
Stack trace:
#0 [internal function]: foo(Object(test))
#1 {main}
