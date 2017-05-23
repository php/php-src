--TEST--
UUIDParsingException::getInput
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

var_dump((new UUIDParsingException('', 'input-test'))->getInput());

?>
--EXPECT--
string(10) "input-test"
