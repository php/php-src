--TEST--
imagecrop() overflows when the combo  x/width or y/height is over INT_MAX or under INT_MIN.
--EXTENSIONS--
gd
--FILE--
<?php
$img = imagecreatetruecolor(10, 10);

$arr = ["x" => 2147483647, "y" => 2147483647, "width" => 10, "height" => 10];

try {
	imagecrop($img, $arr);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

$arr = ["x" => -2147483648, "y" => 0, "width" => -10, "height" => 10];

try {
	imagecrop($img, $arr);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

$arr = ["x" => 1, "y" => 2147483647, "width" => 10, "height" => 10];

try {
	imagecrop($img, $arr);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

$arr = ["x" => 1, "y" => -2147483648, "width" => 10, "height" => -10];

try {
	imagecrop($img, $arr);
} catch (\ValueError $e) {
	echo $e->getMessage();
}
?>
--EXPECT--
imagecrop(): Argument #2 ($rectangle) overflow with "x" and "width" keys
imagecrop(): Argument #2 ($rectangle) underflow with "x" and "width" keys
imagecrop(): Argument #2 ($rectangle) overflow with "y" and "height" keys
imagecrop(): Argument #2 ($rectangle) underflow with "y" and "height" keys
