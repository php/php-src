--TEST--
exception handler tests - 4
--FILE--
<?php

set_exception_handler("fo");
set_exception_handler(array("", ""));
set_exception_handler();
set_exception_handler("foo", "bar");

echo "Done\n";
?>
--EXPECTF--
Warning: set_exception_handler() expects the argument (fo) to be a valid callback in %s on line %d

Warning: set_exception_handler() expects the argument (::) to be a valid callback in %s on line %d

Warning: set_exception_handler() expects exactly 1 parameter, 0 given in %s on line %d

Warning: set_exception_handler() expects exactly 1 parameter, 2 given in %s on line %d
Done
