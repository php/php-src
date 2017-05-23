--TEST--
UUIDParsingException::__construct ArgumentCountError with 0 arguments
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

try {
    new UUIDParsingException;
}
catch (ArgumentCountError $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Too few arguments to method UUIDParsingException::__construct(), 0 passed and at least 2 expected
