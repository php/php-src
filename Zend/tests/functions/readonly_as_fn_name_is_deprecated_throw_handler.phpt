--TEST--
Naming a function readonly is deprecated with an error handler elevating it to an exception
--FILE--
<?php

set_error_handler(function ($number, $message) {
	throw new Exception($message);
});

/* Throwing error handlers do no apply for compile time deprecations */
function readonly() {}

?>
DONE
--EXPECTF--
Deprecated: Calling a function “readonly” is deprecated in %s on line %d
DONE
