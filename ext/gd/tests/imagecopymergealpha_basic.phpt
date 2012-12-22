--TEST--
Testing imagecopymergealpha() of GD library
--CREDITS--
Matt Clegg <cleggmatt [at] gmail [dot] com>
Orignally by: redmonkey
--SKIPIF--
<?php 
if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php

$images=array(
        'imagecopymerge'=>1,
        'imagecopymergealpha'=>2
);

foreach ($images as $f=>$md5){

$bg  = imagecreatefrompng('tux.png');
$over = imagecreatefrompng('ff-logo-sm.png');
imagealphablending($bg, true);
imagesavealpha($bg, true);
$f($bg, $over, 276, 300, 0, 0, 123, 119, 50);

ob_start();
imagepng($bg);
$images[$f] = md5(ob_get_contents());
ob_end_clean();

}

var_dump($images['imagecopymerge']!=$images['imagecopymergealpha']);

?>
--EXPECTF--
bool(true)


