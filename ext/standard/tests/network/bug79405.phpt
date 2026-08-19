--TEST--
Bug #79405 - gethostbyname() silently truncates after a null byte
--FILE--
<?php
$host = "localhost\0.example.com";
try {
	var_dump(gethostbyname($host));
} catch(Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
var_dump(gethostbynamel($host));
} catch(Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: gethostbyname(): Argument #1 ($hostname) must not contain any null bytes
ValueError: gethostbynamel(): Argument #1 ($hostname) must not contain any null bytes
