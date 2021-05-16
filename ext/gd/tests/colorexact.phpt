--TEST--
imagecolorexact
--EXTENSIONS--
gd
--FILE--
<?php

$im = imagecreatetruecolor(5,5);
$c = imagecolorexact($im, 255,0,255);
$c2 = imagecolorexactalpha($im, 255,0,255, 100);

printf("%X\n", $c);
printf("%X\n", $c2);

imagedestroy($im);

$im = imagecreate(5,5);
$c = imagecolorallocate($im, 255,0,255);
$c2 = imagecolorallocate($im, 255,200,0);
$c3 = imagecolorallocatealpha($im, 255,200,0,100);

echo imagecolorexact($im, 255,0,255) . "\n";
echo imagecolorexact($im, 255,200,0) . "\n";
echo imagecolorexactalpha($im, 255,200,0,100) . "\n";


// unallocated index
echo imagecolorexact($im, 12,12,12) . "\n";

imagedestroy($im);
?>
--EXPECT--
FF00FF
64FF00FF
0
1
2
-1
