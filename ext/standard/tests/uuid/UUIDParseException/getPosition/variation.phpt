--TEST--
UUIDParseException::getPosition
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

var_dump((new UUIDParseException('', '', 42))->getPosition());

?>
--EXPECT--
int(42)
