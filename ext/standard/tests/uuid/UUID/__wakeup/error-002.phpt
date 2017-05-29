--TEST--
UUID::__wakeup throws UnexpectedValueException if binary string is not exactly 16 bytes long after deserialization
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

foreach ([0, 1, 15, 17, random_int(18, 256)] as $i) {
	try {
		$bytes = $i === 0 ? '' : random_bytes($i);
		unserialize("O:12:\"PHP\Std\UUID\":1:{s:19:\"\0PHP\Std\UUID\0bytes\";s:{$i}:\"{$bytes}\";}");
	}
	catch (UnexpectedValueException $e) {
		echo $e->getMessage(), "\n";
	}
}

?>
--EXPECTF--
Expected PHP\Std\UUID::$bytes value to be exactly 16 bytes long, but found 0
Expected PHP\Std\UUID::$bytes value to be exactly 16 bytes long, but found 1
Expected PHP\Std\UUID::$bytes value to be exactly 16 bytes long, but found 15
Expected PHP\Std\UUID::$bytes value to be exactly 16 bytes long, but found 17
Expected PHP\Std\UUID::$bytes value to be exactly 16 bytes long, but found %d
