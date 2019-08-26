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
try { 
    $font = imageloadfont($filename);
    $black = imagecolorallocate($image, 0, 0, 0);
    imagestring($image, $font, 0, 0, "Hello", $black);
}
catch (\Throwable $e) {
    echo '!! [Error] ' . $e->getMessage() . "\n";
}

unlink($filename);
?>
--EXPECTF--
Warning: imageloadfont(): product of memory allocation multiplication would exceed INT_MAX, failing operation gracefully
 in %s on line %d
!! [Error] Error reading font, invalid font header
