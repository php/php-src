--TEST--
imagecopy
--SKIPIF--
<?php
        if (!function_exists('imagecopy')) die("skip gd extension not available\n");
?>
--FILE--
<?php

$im = imagecreatetruecolor(5,5);
$c = imagecolorresolve($im, 255,0,255);
printf("%X\n", $c);
imagedestroy($im);

$im = imagecreate(5,5);
$c = imagecolorresolve($im, 255,0,255);
print_r(imagecolorsforindex($im, $c));
imagedestroy($im);

$im = imagecreate(5,5);
for ($i=0; $i<255; $i++) imagecolorresolve($im, $i,0,0);
$c = imagecolorresolve($im, 255,0,0);
print_r(imagecolorsforindex($im, $c));


$im = imagecreate(5,5);
for ($i=0; $i<256; $i++) {
	if ($i == 246) {
		imagecolorresolve($im, $i,10,10);
	} else {
		imagecolorresolve($im, $i,0,0);
	}
}
$c = imagecolorresolve($im, 255,10,10);
print_r(imagecolorsforindex($im, $c));


?>
--EXPECTF--
FF00FF
Array
(
    [red] => 255
    [green] => 0
    [blue] => 255
    [alpha] => 0
)
Array
(
    [red] => 255
    [green] => 0
    [blue] => 0
    [alpha] => 0
)
Array
(
    [red] => 246
    [green] => 10
    [blue] => 10
    [alpha] => 0
)
