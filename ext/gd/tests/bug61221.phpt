--TEST--
Bug #61221 - imagegammacorrect function loses alpha channel
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
$imagew = 50;
$imageh = 50;
$img = imagecreatetruecolor($imagew, $imageh);
$blacktransparent = imagecolorallocatealpha($img, 0, 0, 0, 127);
$redsolid = imagecolorallocate($img, 255, 0, 0);
imagefill($img, 0, 0, $blacktransparent);
imageline($img, $imagew / 2, 0, $imagew / 2, $imageh - 1, $redsolid);
imageline($img, 0, $imageh / 2, $imagew - 1, $imageh / 2, $redsolid);
imagegammacorrect($img, 1, 1);
$color = imagecolorat($img, 0, 0);
var_dump($color === $blacktransparent);
imagedestroy($img);
?>
--EXPECT--
bool(true)
