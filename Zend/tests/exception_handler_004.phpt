--TEST--
exception handler tests - 4
--FILE--
<?php

set_exception_handler("fo");
set_exception_handler(array("", ""));

echo "Done\n";
?>
--EXPECTF--
Warning: set_exception_handler(): Argument #1 ($exception_handler) must be a valid callback in %s on line %d

Warning: set_exception_handler(): Argument #1 ($exception_handler) must be a valid callback in %s on line %d
Done
