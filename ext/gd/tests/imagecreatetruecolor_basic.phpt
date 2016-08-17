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
imagegd($image);
$img = ob_get_contents();
ob_end_clean();

echo md5(base64_encode($img));
?>
--EXPECT--
da884f488a851e0267a316ad232aee86
