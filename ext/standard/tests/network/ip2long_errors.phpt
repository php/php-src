--TEST--
ip2long() error conditions
--FILE--
<?php

try {
	var_dump(ip2long(""));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
	var_dump(ip2long("127\00.0.1"));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
ValueError: ip2long(): Argument #1 ($ip) must not be empty
ValueError: ip2long(): Argument #1 ($ip) must not contain any null bytes

