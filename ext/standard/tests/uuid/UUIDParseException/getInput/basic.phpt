--TEST--
UUIDParseException::getInput
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUIDParseException;

var_dump((new UUIDParseException('', 'input-test'))->getInput());

?>
--EXPECT--
string(10) "input-test"
