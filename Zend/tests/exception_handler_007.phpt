--TEST--
exception handler tests - 1
--FILE--
<?php

var_dump(get_exception_handler());

var_dump(set_exception_handler("my_exception_handler"));

throw new test();

function my_exception_handler($e) {
	var_dump(get_class($e)." thrown!");

	var_dump(restore_exception_handler());

	var_dump(get_exception_handler());
}

class test extends Exception {
}

echo "Done\n";
?>
--EXPECTF--
NULL
NULL
string(12) "test thrown!"
bool(true)
NULL
