--TEST--
Bug #66356 (Heap Overflow Vulnerability in imagecrop())
--EXTENSIONS--
gd
--FILE--
<?php
$img = imagecreatetruecolor(10, 10);

// POC #1
var_dump(imagecrop($img, array("x" => 0, "y" => 0, "width" => 10, "height" => 10)));

$arr = array("x" => 2147483647, "y" => 2147483647, "width" => 10, "height" => 10);
try {
	imagecrop($img, $arr);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
print_r($arr);

// POC #2
var_dump(imagecrop($img, array("x" => 0, "y" => 0, "width" => -1, "height" => 10)));

// POC #3
var_dump(imagecrop($img, array("x" => -20, "y" => -20, "width" => 10, "height" => 10)));

// POC #4
var_dump(imagecrop($img, array("x" => 0x7fffff00, "y" => 0, "width" => 10, "height" => 10)));

// bug 66815
var_dump(imagecrop($img, array("x" => 0, "y" => 0, "width" => 65535, "height" => 65535)));
?>
--EXPECTF--
object(GdImage)#2 (0) {
}
imagecrop(): Argument #2 ($rectangle) overflow with "x" and "width" keys
Array
(
    [x] => 2147483647
    [y] => 2147483647
    [width] => 10
    [height] => 10
)

Warning: imagecrop(): %cne parameter to a memory allocation multiplication is negative or zero, failing operation gracefully
 in %s on line %d
bool(false)
object(GdImage)#3 (0) {
}
object(GdImage)#3 (0) {
}

Warning: imagecrop(): %croduct of memory allocation multiplication would exceed INT_MAX, failing operation gracefully
 in %s on line %d
bool(false)
