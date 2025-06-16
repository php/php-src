--TEST--
GH-17772 (imagepalettetotruecolor segfault on image deallocation)
--EXTENSIONS--
gd
--INI--
memory_limit=2M
--CREDITS--
YuanchengJiang
--SKIPIF--
<?php
if (!GD_BUNDLED) die("skip requires bundled GD library");
?>
--FILE--
<?php
function setStyleAndThickness($im, $color, $thickness)
{
$arr = [];
$i = 0;
while ($i < 16 * $thickness) {
$arer[$i++] = $color;
}
}
$im = imagecreate(800, 800);
setStyleAndThickness($im, 0, 6);
imagepalettetotruecolor($im);
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
