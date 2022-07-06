--TEST--
libgd #186 (Tiling true colour with palette image does not work)
--SKIPIF--
<?php
    if (!extension_loaded('gd')) die("skip gd extension not available\n");
?>
--FILE--
<?php
$im = imagecreatetruecolor(10,10);
$tile = imagecreate(10,10);
$red   = imagecolorallocate($tile,0xff,0,0);
$green = imagecolorallocate($tile,0,0xff,0);
$blue  = imagecolorallocate($tile,0,0,0xff);
$other = imagecolorallocate($tile,0,0,0x2);
imagefilledrectangle($tile,0,0,2,10,$red);
imagefilledrectangle($tile,3,0,4,10,$green);
imagefilledrectangle($tile,5,0,7,10,$blue);
imagefilledrectangle($tile,8,0,9,10,$other);
imagecolortransparent($tile,$blue);
imagesettile($im,$tile);
for ($i=0; $i<10; $i++) {
  imagesetpixel($im,$i,$i,IMG_COLOR_TILED);
}
$index = imagecolorat($im,9,9);
$arr = imagecolorsforindex($im, $index);
if ($arr['blue'] == 2) {
  $r = "Ok";
} else {
  $r = "Failed";
}
imagedestroy($tile);
imagedestroy($im);
echo $r;
?>
--EXPECT--
Ok
