--TEST--
exception handler tests - 7
--FILE--
<?php

set_exception_handler("foo");
set_exception_handler(null);

function foo($e) {
    var_dump(__FUNCTION__."(): ".get_class($e)." thrown!");
}

function foo1($e) {
    var_dump(__FUNCTION__."(): ".get_class($e)." thrown!");
}


throw new excEption();

echo "Done\n";
?>
--EXPECTF--
Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
