--TEST--
exception handler tests - 4
--FILE--
<?php

set_exception_handler("fo");
set_exception_handler(array("", ""));

echo "Done\n";
?>
--EXPECTF--
Warning: set_exception_handler() expects the argument (fo) to be a valid callback in %s on line %d

Warning: set_exception_handler() expects the argument (::) to be a valid callback in %s on line %d
Done
