--TEST--
GH-16232 (Overflow on reading wbmp content)
--EXTENSIONS--
gd
--FILE--
<?php
$good_webp = __DIR__ . '/src.wbmp';
$bad_webp = __DIR__ . "/gh16232.webp";
copy($good_webp, $bad_webp);
var_dump(imagecreatefromwbmp($bad_webp));
$data = file_get_contents($bad_webp);
$data[3] = chr(-1);
file_put_contents($bad_webp, $data);
var_dump(imagecreatefromwbmp($bad_webp));
$data[3] = chr(1000);
file_put_contents($bad_webp, $data);
var_dump(imagecreatefromwbmp($bad_webp));
unlink($bad_webp);
--EXPECTF--
object(GdImage)#1 (0) {
}

Warning: imagecreatefromwbmp(): "%s" is not a valid WBMP file in %s on line %d
bool(false)

Warning: imagecreatefromwbmp(): "%s" is not a valid WBMP file in %s on line %d
bool(false)
