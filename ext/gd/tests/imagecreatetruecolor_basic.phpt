--TEST--
Testing imagecreatetruecolor() of GD library
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--SKIPIF--
<?php 
	if (!extension_loaded("gd")) die("skip GD not present");
	if (!function_exists("imagecreatetruecolor")) die("skip GD Version not compatible");
?>
--FILE--
<?php
$image = imagecreatetruecolor(180, 30);

ob_start();
imagepng($image, null, 9);
$img = ob_get_contents();
ob_end_clean();

echo md5(base64_encode($img));
?>
--EXPECT--
5a8fe9864cbd20e5dbe730c77f30db95
