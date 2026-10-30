--TEST--
 #72337	segfault in imagescale with new dimensions being <=0)
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreatetruecolor(1, 1);
try {
	imagescale($im, 1, 1, -10);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	imagescale($im, 0, 1, 0);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	imagescale($im, 1, 0, 0);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
imagescale($im, 1, 1, IMG_BICUBIC_FIXED);
echo "OK";
?>
--EXPECT--
imagescale(): Argument #4 ($mode) must be one of the GD_* constants
imagescale(): Argument #2 ($width) must be between 1 and 2147483647
imagescale(): Argument #3 ($height) must be between 1 and 2147483647
OK
