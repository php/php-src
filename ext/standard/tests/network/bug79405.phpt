--TEST--
Bug #79405 - gethostbyname() silently truncates after a null byte
--FILE--
<?php
$host = "localhost\0.example.com";
try {
	var_dump(gethostbyname($host));
} catch(Error $e) {
	print $e->getMessage()."\n";
}
try {
var_dump(gethostbynamel($host));
} catch(Error $e) {
	print $e->getMessage()."\n";
}
?>
--EXPECT--
gethostbyname(): Argument #1 ($hostname) must not contain any null bytes
gethostbynamel(): Argument #1 ($hostname) must not contain any null bytes