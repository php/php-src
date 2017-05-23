--TEST--
UUIDParsingException::getPosition
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

var_dump((new UUIDParsingException('', '', 42))->getPosition());

?>
--EXPECT--
int(42)
