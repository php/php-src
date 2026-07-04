--TEST--
Bug #19366 (gdimagefill() function crashes (fixed in bundled libgd))
--EXTENSIONS--
gd
--FILE--
<?php
echo "Alive: create image\n";
$width = 50;
$height = 100;
$ImHandle = imagecreate($width,$height);

echo "Alive: Define colors\n";
$MyBlue = imagecolorallocate($ImHandle, 0, 0, 255);
$MyRed = imagecolorallocate($ImHandle, 255, 0, 0);
$MyWhite = imagecolorallocate($ImHandle, 255, 255, 255);
$MyBlack = imagecolorallocate($ImHandle, 0, 0, 0);

echo "Alive: Draw\n";
imagefill($ImHandle,0,0,$MyBlack);
imageline($ImHandle,20,20,180,20,$MyWhite);
imageline($ImHandle,20,20,20,70,$MyBlue);
imageline($ImHandle,20,70,180,70,$MyRed);
imageline($ImHandle,180,20,180,45,$MyWhite);
imageline($ImHandle,180,70,180,45,$MyRed);
imageline($ImHandle,20,20,100,45,$MyBlue);
imageline($ImHandle,20,70,100,45,$MyBlue);
imageline($ImHandle,100,45,180,45,$MyRed);
imagefill($ImHandle,21,45,$MyBlue);
imagefill($ImHandle,100,69,$MyRed);
imagefill($ImHandle,100,21,$MyWhite);

echo "Alive: ImageString\n";
imagestring($ImHandle,4,40,75,"Czech Republic",$MyWhite);

echo "Alive: Send to browser\n";
//Header("Content-type: image/PNG");
//imagepng($ImHandle);

echo "Alive: Done\n";
?>
--EXPECT--
Alive: create image
Alive: Define colors
Alive: Draw
Alive: ImageString
Alive: Send to browser
Alive: Done
