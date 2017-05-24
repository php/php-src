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
UUIDParseException::__construct() expects at least 2 parameters, 0 given
