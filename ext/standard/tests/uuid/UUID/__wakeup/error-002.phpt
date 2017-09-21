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
Expected UUID::$bytes value to be exactly 16 bytes long, but found 0
Expected UUID::$bytes value to be exactly 16 bytes long, but found 1
Expected UUID::$bytes value to be exactly 16 bytes long, but found 15
Expected UUID::$bytes value to be exactly 16 bytes long, but found 17
Expected UUID::$bytes value to be exactly 16 bytes long, but found %d
