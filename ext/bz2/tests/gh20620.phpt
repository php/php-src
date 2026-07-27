--TEST--
Bug GH-20620 (bzcompress with large source)
--EXTENSIONS--
bz2
--SKIPIF--
<?php 
if (!getenv('RUN_RESOURCE_HEAVY_TESTS')) die('skip resource-heavy test');
if (PHP_INT_SIZE != 8) die('skip this test is for 64bit platforms only'); 
if (getenv('SKIP_SLOW_TESTS')) die('skip slow tests excluded by request');
?>
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
bzcompress(): Argument #1 ($data) must have a length less than or equal to %d
