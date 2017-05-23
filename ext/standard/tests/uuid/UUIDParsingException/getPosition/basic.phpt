--TEST--
UUIDParsingException::getPosition default value
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

var_dump((new UUIDParsingException('', ''))->getPosition());

?>
--EXPECT--
int(0)
