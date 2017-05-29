--TEST--
UUID::parse throws UUIDParseException if too many hexadecimal digits are found
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUID;
use PHP\Std\UUIDParseException;

try {
	UUID::parse('12345678-1234-1234-1234-123456789abcdef');
}
catch (UUIDParseException $e) {
	echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Expected no more than 32 hexadecimal digits
