--TEST--
gdimagefill() function (Bug #19366 (fixed in bundled libgd))
--SKIPIF--
<?php 
	if (!extension_loaded('gd')) die("skip gd extension not available\n"); 
	if (!GD_BUNDLED) die('skip external GD libraries always fail');
?>
--FILE--
<?php
echo "Alive: create image\n";
$width = 50;
$height = 100;
$ImHandle = imagecreate($width,$height);

echo "Alive: Define colors\n";
$MyBlue = imagecolorAllocate($ImHandle, 0, 0, 255);
$MyRed = imagecolorAllocate($ImHandle, 255, 0, 0);
$MyWhite = imagecolorAllocate($ImHandle, 255, 255, 255);
$MyBlack = imagecolorAllocate($ImHandle, 0, 0, 0);

echo "Alive: Draw\n";
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

echo "Alive: ImageString\n";
ImageString($ImHandle,4,40,75,"Czech Republic",$MyWhite);

echo "Alive: Send to browser\n";
//Header("Content-type: image/PNG");
//ImagePNG($ImHandle);

echo "Alive: Free resources\n";
imagedestroy($ImHandle);
echo "Alive: Done\n";
?>
--EXPECT--
Alive: create image
Alive: Define colors
Alive: Draw
Alive: ImageString
Alive: Send to browser
Alive: Free resources
Alive: Done
