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
Too few arguments to method UUIDParseException::__construct(), 1 passed and at least 2 expected
