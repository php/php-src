--TEST--
imagecopy
--SKIPIF--
<?php
        if (!function_exists('imagecopy')) die("skip gd extension not available\n");
?>
--FILE--
<?php

$im = imagecreatetruecolor(5,5);
$c = imagecolorclosest($im, 255,0,255);
printf("%X\n", $c);
imagedestroy($im);

$im = imagecreate(5,5);
$c = imagecolorclosest($im, 255,0,255);
print_r(imagecolorsforindex($im, $c));
imagedestroy($im);



$im = imagecreate(5,5);
imagecolorallocate($im, 255, 0, 255);
$c = imagecolorclosest($im, 255,0,255);
print_r(imagecolorsforindex($im, $c));
imagedestroy($im);

$im = imagecreate(5,5);
for ($i=0; $i<255; $i++) imagecolorresolve($im, $i,0,0);
$c = imagecolorclosest($im, 255,0,0);
print_r(imagecolorsforindex($im, $c));


$im = imagecreate(5,5);
for ($i=0; $i<256; $i++) {
	if ($i == 246) {
		imagecolorallocate($im, $i,10,10);
	} else {
		imagecolorallocate($im, $i,0,0);
	}
}
$c = imagecolorclosest($im, 255,10,10);
print_r(imagecolorsforindex($im, $c));


?>
--EXPECTF--
FF00FF

Warning: imagecolorsforindex(): Color index -1 out of range in %s on line %d
Array
(
    [red] => 255
    [green] => 0
    [blue] => 255
    [alpha] => 0
)
Array
(
    [red] => 254
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
