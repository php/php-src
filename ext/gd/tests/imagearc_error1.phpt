--TEST--
Testing wrong param passing imagearc() of GD library
--CREDITS--
Edgar Ferreira da Silva <contato [at] edgarfs [dot] com [dot] br>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php 
if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php

$image = imagecreatetruecolor(100, 100);

$white = imagecolorallocate($image, 0xFF, 0xFF, 0xFF);

//create an arc with white color    
imagearc($image, 50, 50, 30, 30, 0, 180);

ob_start();
imagepng($image);
$img = ob_get_contents();
ob_end_clean();

echo md5(base64_encode($img));
?>
--EXPECTF--
Warning: imagearc() expects exactly 8 parameters, 7 given in %s on line %d
abebb25b5a2813cfbf92f1f24365786a
