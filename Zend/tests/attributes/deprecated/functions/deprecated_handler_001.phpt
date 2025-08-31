--TEST--
#[\Deprecated]: Exception Handler is deprecated.
--FILE--
<?php

#[\Deprecated]
function my_exception_handler($e) {
	echo "Handled: ", $e->getMessage(), PHP_EOL;
};

set_exception_handler('my_exception_handler');

throw new \Exception('test');

?>
--EXPECT--
Deprecated: Function my_exception_handler() is deprecated in Unknown on line 0
Handled: test
