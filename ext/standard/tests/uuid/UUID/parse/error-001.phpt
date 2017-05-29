--TEST--
UUID::parse throws UUIDParseException if there are less than 32 chars
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUID;
use PHP\Std\UUIDParseException;

foreach (
	[
		'',
		'0',
		str_repeat('0', 31),
		'0123456789-0123456789-0123456789',
		" \t{012345678901234567890123456789}\t ",
	] as $date
) {
	try {
		UUID::parse($date);
	}
	catch (UUIDParseException $e) {
		echo $e->getMessage(), "\n";
	}
}

?>
--EXPECT--
Expected at least 32 hexadecimal digits, but got 0
Expected at least 32 hexadecimal digits, but got 1
Expected at least 32 hexadecimal digits, but got 31
Expected at least 32 hexadecimal digits, but got 30
Expected at least 32 hexadecimal digits, but got 30
