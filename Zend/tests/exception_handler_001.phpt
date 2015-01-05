--TEST--
exception handler tests - 1
--FILE--
<?php

set_exception_handler("foo");

function foo($e) {
	var_dump(get_class($e)." thrown!");
}

class test extends Exception {
}

throw new test();

echo "Done\n";
?>
--EXPECTF--	
string(12) "test thrown!"
