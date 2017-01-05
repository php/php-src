--TEST--
Testing wrong param passing imageellipse() of GD library
--CREDITS--
Ivan Rosolen <contato [at] ivanrosolen [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php 
if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php

// Create a image
$image = imagecreatetruecolor(400, 300);

// try to draw a white ellipse
imageellipse($image, 'wrong param', 150, 300, 200, 16777215);

?>
--EXPECTF--
Warning: imageellipse() expects parameter 2 to be integer, %s given in %s on line %d
