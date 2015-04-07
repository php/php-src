--TEST--
Bug #60738 Allow 'set_error_handler' to handle NULL
--FILE--
<?php

var_dump(set_exception_handler(
    function() { echo 'Intercepted exception!', "\n"; }
));

var_dump(set_exception_handler(null));

throw new Exception('Exception!');
?>
--EXPECTF--
NULL
object(Closure)#1 (0) {
}

Exception: Exception! in %s on line %d
Stack trace:
#0 {main}

