--TEST--
Testing imagefilledellipse() of GD library
--SKIPIF--
<?php 
if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php

$image = imagecreatetruecolor(100, 100);

$white = imagecolorallocate($image, 0xFF, 0xFF, 0xFF);

//create an ellipse and fill it with white color    
imagefilledellipse($image, 50, 50, 40, 30, $white);

ob_start();
imagepng($image);
$img = ob_get_contents();
ob_end_clean();

echo md5(base64_encode($img));
?>
--EXPECT--
9ba540bba1b78c9f08efaa6fa0afd93b
