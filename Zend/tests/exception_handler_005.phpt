--TEST--
exception handler tests - 5
--FILE--
<?php

set_exception_handler("foo");
set_exception_handler("foo1");

function foo($e) {
	var_dump(__FUNCTION__."(): ".get_class($e)." thrown!");
}

function foo1($e) {
	var_dump(__FUNCTION__."(): ".get_class($e)." thrown!");
}


throw new excEption();

echo "Done\n";
?>
--EXPECT--
string(25) "foo1(): Exception thrown!"
