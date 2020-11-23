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

try {
    imagecolormatch($im, $im2);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "ok\n";

imagedestroy($im);
?>
--EXPECT--
imagecolormatch(): Argument #2 ($image2) must have at least one color
ok
