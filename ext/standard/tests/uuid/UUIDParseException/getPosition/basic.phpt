--TEST--
UUIDParseException::getPosition default value
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUIDParseException;

var_dump((new UUIDParseException('', ''))->getPosition());

?>
--EXPECT--
int(0)
