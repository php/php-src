--TEST--
UUIDParseException::__construct ArgumentCountError with 0 arguments
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

try {
    new UUIDParseException;
}
catch (ArgumentCountError $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Too few arguments to method UUIDParseException::__construct(), 0 passed and at least 2 expected
