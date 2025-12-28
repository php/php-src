--TEST--
GH-16559 (UBSan abort in ext/gd/libgd/gd_interpolation.c:1007)
--EXTENSIONS--
gd
--FILE--
<?php
$input = imagecreatefrompng(__DIR__ . '/gh10614.png');
for ($angle = 0; $angle <= 270; $angle += 90) {
	$output = imagerotate($input, $angle, 0);
}
var_dump(imagescale($output, -1, 64, IMG_BICUBIC));
?>
--EXPECT--
object(GdImage)#2 (0) {
}
