--TEST--
gdimagefill() function (Bug #19366 (in bundled libgd))
--SKIPIF--
<?php if (!extension_loaded('gd')) die("skip\n"); ?>
--POST--
--GET--
--INI--
--FILE--
<?php
echo "Alive\n";
$width = 50;
$height = 100;
$ImHandle = imagecreate($width,$height);
echo "Alive\n";
//Define colors
$MyBlue = imagecolorAllocate($ImHandle, 0, 0, 255);
$MyRed = imagecolorAllocate($ImHandle, 255, 0, 0);
$MyWhite = imagecolorAllocate($ImHandle, 255, 255, 255);
$MyBlack = imagecolorAllocate($ImHandle, 0, 0, 0);
echo "Alive\n";
//Draw
ImageFill($ImHandle,0,0,$MyBlack);
ImageLine($ImHandle,20,20,180,20,$MyWhite);
ImageLine($ImHandle,20,20,20,70,$MyBlue);
ImageLine($ImHandle,20,70,180,70,$MyRed);
ImageLine($ImHandle,180,20,180,45,$MyWhite);
ImageLine($ImHandle,180,70,180,45,$MyRed);
ImageLine($ImHandle,20,20,100,45,$MyBlue);
ImageLine($ImHandle,20,70,100,45,$MyBlue);
ImageLine($ImHandle,100,45,180,45,$MyRed);
ImageFill($ImHandle,21,45,$MyBlue);
ImageFill($ImHandle,100,69,$MyRed);
ImageFill($ImHandle,100,21,$MyWhite);
ImageString($ImHandle,4,40,75,"Czech Republic",$MyWhite);
echo "Alive\n";
// Send to browser
Header("Content-type: image/PNG");
//ImagePNG($ImHandle);
echo "Alive\n";
//Free resources
imagedestroy($ImHandle);
echo "Alive\n";
?>
--EXPECT--
Alive
Alive
Alive
Alive
Alive
Alive
