--TEST--
imagecrop/imageaffine matrix checks
--EXTENSIONS--
gd
--FILE--
<?php
$img = imagecreatetruecolor(10, 10);

try {
	imagecrop($img, array("x" => PHP_INT_MAX, "y" => 10, "width" => 10, "height" => 105));
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	imageaffine($img, array(1, 2, 3, 4, 5, 6), array("x" => 120, "y" => 180, "width" => PHP_INT_MAX, "height" => 10));
} catch (\ValueError $e) {
	echo $e->getMessage();
}
?>
--EXPECT--
imagecrop(): Argument #2 ($rectangle) invalid "x" key value
imageaffine(): Argument #3 ($clip) invalid "width" key value
