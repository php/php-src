--TEST--
UUIDParseException::__construct ArgumentCountError with 1 argument
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

try {
	new UUIDParseException('reason');
}
catch (ArgumentCountError $e) {
	echo $e->getMessage();
}

?>
--EXPECT--
UUIDParseException::__construct() expects at least 2 parameters, 1 given
