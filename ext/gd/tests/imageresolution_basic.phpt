--TEST--
Wrong image resolution
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip on non 64 bits architectures");
?>
--FILE--
<?php
$filename = __DIR__ . DIRECTORY_SEPARATOR . 'imageresolution_png.png';

$exp = imagecreate(100, 100);
imagecolorallocate($exp, 255, 127, 64);

$res = imageresolution($exp);

try {
	imageresolution($exp, PHP_INT_MAX);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	imageresolution($exp, 127, -PHP_INT_MAX);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
imageresolution($exp, 0, 0);
var_dump(imageresolution($exp) == $res);
?>
--EXPECTF--
imageresolution(): Argument #2 ($resolution_x) must be between 0 and %d
imageresolution(): Argument #3 ($resolution_y) must be between 0 and %d
bool(true)
