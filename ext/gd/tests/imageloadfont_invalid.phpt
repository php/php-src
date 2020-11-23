--TEST--
imageloadfont() function crashes
--SKIPIF--
<?php
    if (!extension_loaded('gd')) die("skip gd extension not available\n");
?>
--FILE--
<?php
$filename = __DIR__ .  '/font.gdf';
$bin = "\x41\x41\x41\x41\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x01\x00";
$fp = fopen($filename, 'wb');
fwrite($fp, $bin);
fclose($fp);

$image = imagecreatetruecolor(50, 20);
$font = imageloadfont($filename);
$black = imagecolorallocate($image, 0, 0, 0);
imagestring($image, $font, 0, 0, "Hello", $black);
unlink($filename);
?>
--EXPECTF--
Warning: imageloadfont(): Product of memory allocation multiplication would exceed INT_MAX, failing operation gracefully
 in %simageloadfont_invalid.php on line %d

Warning: imageloadfont(): Error reading font, invalid font header in %simageloadfont_invalid.php on line %d
