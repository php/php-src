--TEST--
GH-20551: (imagegammacorrect out of range input/output value)
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreatetruecolor(16, 16);

try {
	imagescale($im, PHP_INT_MAX, -1);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	imagescale($im, -1, PHP_INT_MAX);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
imagescale(): Argument #2 ($width) must be less than or equal to %d
imagescale(): Argument #3 ($height) must be less than or equal to %d
