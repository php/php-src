--TEST--
GH-18005: imagesetstyle, imagefilter, imagecrop array values type checks
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die('skip this test is for 64bit platforms only');
?>
--FILE--
<?php
class A {}
$img = imagecreatetruecolor(1, 1);
try {
	imagesetstyle($img, [0, new A()]);
} catch (\TypeError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	imagesetstyle($img, [0, PHP_INT_MIN]);
} catch (\TypeError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	imagefilter($img, IMG_FILTER_SCATTER, 0, 0, [new A()]);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	imagefilter($img, IMG_FILTER_SCATTER, 0, 0, [-1]);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	imagecrop($img, ["x" => PHP_INT_MIN, "y" => 0, "width" => 0, "height" => 0]);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	imagecrop($img, ["x" => 0, "y" => PHP_INT_MIN, "width" => 0, "height" => 0]);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	imagecrop($img, ["x" => 0, "y" => 0, "width" => PHP_INT_MAX, "height" => 0]);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	imagecrop($img, ["x" => 0, "y" => 0, "width" => 0, "height" => PHP_INT_MAX]);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	imagecrop($img, ["x" => new A(), "y" => 0, "width" => 0, "height" => 0]);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	imagecrop($img, ["x" => 0, "y" => new A(), "width" => 0, "height" => 0]);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	imagecrop($img, ["x" => 0, "y" => 0, "width" => new A(), "height" => 0]);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	imagecrop($img, ["x" => 0, "y" => 0, "width" => 0, "height" => new A()]);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

$one = 1;
var_dump(imagecrop($img, ["x" => &$one, "y" => &$one, "width" => &$one, "height" => &$one]));
?>
--EXPECTF--
imagesetstyle(): Argument #2 ($style) must only have elements of type int, A given
imagesetstyle(): Argument #2 ($style) elements must be between %i and %d
imagefilter(): Argument #5 value must be of type int, A given
imagefilter(): Argument #5 value must be between 0 and 2147483647
imagecrop(): Argument #2 ($rectangle) "x" key must be between %i and %d
imagecrop(): Argument #2 ($rectangle) "y" key must be between %i and %d
imagecrop(): Argument #2 ($rectangle) "width" key must be between %i and %d
imagecrop(): Argument #2 ($rectangle) "height" key must be between %i and %d
imagecrop(): Argument #2 ($rectangle) "x" key must be of type int, A given
imagecrop(): Argument #2 ($rectangle) "y" key must be of type int, A given
imagecrop(): Argument #2 ($rectangle) "width" key must be of type int, A given
imagecrop(): Argument #2 ($rectangle) "height" key must be of type int, A given
object(GdImage)#2 (0) {
}
