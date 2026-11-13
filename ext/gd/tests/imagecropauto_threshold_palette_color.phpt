--TEST--
imagecropauto(): threshold mode, a color outside the palette
--EXTENSIONS--
gd
--FILE--
<?php
$p = imagecreate(6, 6);
imagecolorallocate($p, 255, 255, 255);
imagecolorallocate($p, 10, 10, 10);
foreach ([imagecolorstotal($p), -1, 1] as $color) {
    try {
        var_dump(imagecropauto($p, IMG_CROP_THRESHOLD, 0.5, $color) !== false);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
ValueError: imagecropauto(): Argument #4 ($color) must be a valid color index of the image when using the threshold mode
ValueError: imagecropauto(): Argument #4 ($color) must be greater than or equal to 0 when using the threshold mode
bool(true)
