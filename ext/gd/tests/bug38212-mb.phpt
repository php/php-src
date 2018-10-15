--TEST--
Bug #38212 (Seg Fault on invalid imagecreatefromgd2part() parameters)
--SKIPIF--
<?php
        if (!function_exists('imagecopy')) die("skip gd extension not available\n");
?>
--FILE--
<?php
$file = dirname(__FILE__) . '/bug38212私はガラスを食べられます.gd2';
$im1 = imagecreatetruecolor(10,100);
imagefill($im1, 0,0, 0xffffff);
imagegd2($im1, $file);
$im = imagecreatefromgd2part($file, 0,0, -25,10);
unlink($file);
?>
--EXPECTF--
Warning: imagecreatefromgd2part(): Zero width or height not allowed in %s on line %d
