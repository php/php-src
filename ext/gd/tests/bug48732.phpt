--TEST--
Bug #48732 (TTF Bounding box wrong for letters below baseline)
--EXTENSIONS--
gd

--SKIPIF--
<?php
    if(!function_exists('imagefttext')) die('skip imagefttext() not available');
    if (!(imagetypes() & IMG_PNG)) {
        die("skip No PNG support");
    }
?>
--FILE--
<?php
$cwd = __DIR__;
$font = "$cwd/Tuffy.ttf";
$g = imagecreate(100, 50);
$bgnd  = imagecolorallocate($g, 255, 255, 255);
$black = imagecolorallocate($g, 0, 0, 0);
$bbox  = imagettftext($g, 12, 0, 0, 20, $black, $font, "ABCEDFGHIJKLMN\nopqrstu\n");
imagepng($g, "$cwd/bug48732.png");
echo 'Left Bottom: (' . $bbox[0]  . ', ' . $bbox[1] . ')';
?>
--CLEAN--
<?php @unlink(__DIR__ . '/bug48732.png'); ?>
--EXPECT--
Left Bottom: (0, 46)
