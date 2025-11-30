--TEST--
Bug GH-20620 (bzcompress with large source)
--EXTENSIONS--
bz2
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die('skip this test is for 64bit platforms only'); ?>
--INI--
memory_limit=-1
--FILE--
<?php
try {
	bzcompress(str_repeat('1', 4295163906));
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
bzcompress(): Argument #1 ($data) must be less than or equal to %d
