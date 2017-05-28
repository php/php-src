--TEST--
UUID::__wakeup throws UnexpectedValueException if binary string is not exactly 16 bytes long after deserialization
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

foreach ([0, 1, 15, 17, random_int(18, 256)] as $i) {
	try {
		$bytes = $i === 0 ? '' : random_bytes($i);
		unserialize("O:4:\"UUID\":1:{s:11:\"\0UUID\0bytes\";s:{$i}:\"{$bytes}\";}");
	}
	catch (UnexpectedValueException $e) {
		echo $e->getMessage(), "\n";
	}
}

?>
--EXPECTF--
Expected exactly 16 bytes, but found 0
Expected exactly 16 bytes, but found 1
Expected exactly 16 bytes, but found 15
Expected exactly 16 bytes, but found 17
Expected exactly 16 bytes, but found %d
