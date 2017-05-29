--TEST--
UUID::parse throws UUIDParseException for non-hexadecimal characters
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUID;
use PHP\Std\UUIDParseException;

try {
	UUID::parse('01234567-89ab-cdef-0123-456789abcPHP');
}
catch (UUIDParseException $e) {
	echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Expected hexadecimal digit, but found 'P' (0x50)
