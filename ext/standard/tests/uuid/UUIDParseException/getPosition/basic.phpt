--TEST--
UUIDParseException::getPosition default value
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

var_dump((new UUIDParseException('', ''))->getPosition());

?>
--EXPECT--
int(0)
