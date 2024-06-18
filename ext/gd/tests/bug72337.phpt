--TEST--
 #72337	segfault in imagescale with new dimensions being <=0)
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreatetruecolor(1, 1);
try {
	imagescale($im, 0, 0, -10);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
imagescale($im, 0, 0, IMG_BICUBIC_FIXED);
echo "OK";
?>
--EXPECT--
imagescale(): Argument #4 ($mode) must be a valid mode
OK
