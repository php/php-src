--TEST--
Bug #73957 (signed integer conversion in imagescale())
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die('skip this test is for 64bit platforms only');
?>
--FILE--
<?php
$im = imagecreate(8, 8);

try {
	$im = imagescale($im, 0x100000001, 1);
	// which is not supposed to happen
	var_dump(imagesx($im));
} catch (\ValueError $e) {
	echo $e->getMessage();
}
?>
--EXPECTF--
imagescale(): Argument #2 ($width) must be between 1 and %d
