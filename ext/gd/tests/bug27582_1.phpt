--TEST--
Bug #27582 (ImageFillToBorder() on alphablending image looses alpha on fill color)
--SKIPIF--
<?php
        if (!extension_loaded('gd')) die("skip gd extension not available\n");
        if (!GD_BUNDLED) die('skip external GD libraries always fail');
?>
--FILE--
<?php
/* $id$ */
$dest = dirname(realpath(__FILE__)) . '/bug27582.png';
@unlink($dest);
$im = ImageCreateTrueColor(10, 10);
imagealphablending($im, true);
imagesavealpha($im, false);
$bordercolor=ImageColorAllocateAlpha($im, 0, 0, 0, 2);
$color = ImageColorAllocateAlpha($im, 0, 0, 0, 1);
ImageFillToBorder($im, 5, 5, $bordercolor, $color);
imagepng($im, $dest);
echo md5_file($dest) . "\n";
@unlink($dest);
?>
--EXPECT--
08287f8f5d406946009df5f04ca83dc0
