--TEST--
GH-17463 segfault on SplFileObject::ftruncate() with negative value.
--CREDITS--
YuanchengJiang
--FILE--
<?php
$cls = new SplTempFileObject();

try {
	$cls->ftruncate(-1);
} catch (\ValueError $e) {
	echo $e->getMessage();
}
?>
--EXPECT--
SplFileObject::ftruncate(): Argument #1 ($size) must be greater than or equal to 0
