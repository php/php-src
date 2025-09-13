--TEST--
Bug #70963 (Unserialize shows UNKNOW in result)
--FILE--
<?php
try {
	var_dump(unserialize('a:2:{i:0;O:9:"exception":1:{s:16:"'."\0".'Exception'."\0".'trace";s:4:"test";}i:1;R:3;}'));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
	var_dump(unserialize('a:2:{i:0;O:9:"exception":1:{s:16:"'."\0".'Exception'."\0".'trace";s:4:"test";}i:1;r:3;}'));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
TypeError: Cannot assign string to property Exception::$trace of type array

Warning: unserialize(): Error at offset 72 of 73 bytes in %s on line %d
TypeError: Cannot assign string to property Exception::$trace of type array
