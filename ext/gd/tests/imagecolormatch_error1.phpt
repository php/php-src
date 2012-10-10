--TEST--
Send only 1 parameter imagecolormatch() of GD library
--CREDITS--
Paulo Alves de Sousa Filho <pspalves [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php 
if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php
$ima = imagecreatetruecolor(110, 20);
$background_color = imagecolorallocate($ima, 0, 0, 0);
var_dump(imagecolormatch($ima));
?>
--EXPECTF--
Warning: imagecolormatch() expects exactly 2 parameters, %d given in %s on line %d
NULL
