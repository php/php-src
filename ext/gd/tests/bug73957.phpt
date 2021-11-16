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
$im = imagescale($im, 0x100000001, 1);
var_dump($im);
if ($im) { // which is not supposed to happen
    var_dump(imagesx($im));
}
?>
--EXPECT--
bool(false)
