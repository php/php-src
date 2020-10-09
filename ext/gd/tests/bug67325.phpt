--TEST--
Bug #67325 (imagetruecolortopalette: white is duplicated in palette)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.3', '<=')) {
    die("skip test requires newer than GD 2.2.3");
}
 if (!(imagetypes() & IMG_JPG)) die('skip JPEG support not available');
?>
--FILE--
<?php
$filename = __DIR__ . DIRECTORY_SEPARATOR . 'bug67325.jpg';

$im = imagecreatefromjpeg($filename);
imagetruecolortopalette($im, 0, 256);

$white = 0;
for ($i = 0; $i < 256; $i++) {
    $components = imagecolorsforindex($im, $i);
    if ($components['red'] === 255 && $components['green'] === 255 && $components['blue'] === 255) {
        $white++;
    }
}
var_dump($white);

imagedestroy($im);
?>
--EXPECT--
int(0)
