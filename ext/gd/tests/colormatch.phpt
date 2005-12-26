--TEST--
imagecolormatch
--SKIPIF--
<?php
        if (!function_exists('imagecolormatch')) die("skip gd extension not available\n");
?>
--FILE--
<?php

$im = imagecreatetruecolor(5,5);
$im2 = imagecreate(5,5);

imagecolormatch($im, $im2);

echo "ok\n";

imagedestroy($im);
?>
--EXPECTF--
Warning: imagecolormatch(): Image2 must have at least one color in %s on line %d
ok
