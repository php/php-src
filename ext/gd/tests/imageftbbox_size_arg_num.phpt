--TEST--
The $size errors name the argument of the function that was called
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!function_exists('imageftbbox')) die('skip imageftbbox() not available');
?>
--FILE--
<?php
$font = __DIR__ . '/Rochester-Regular.otf';
$image = imagecreatetruecolor(100, 80);

/* $size is argument #1 here */
foreach ([NAN, INF, PHP_INT_MAX, PHP_INT_MIN] as $size) {
    try {
        imageftbbox($size, 0.0, $font, 'A');
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
    try {
        imagettfbbox($size, 0.0, $font, 'A');
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}

/* and argument #2 here */
foreach ([NAN, INF] as $size) {
    try {
        imagefttext($image, $size, 0.0, 15, 60, 0, $font, 'A');
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
    try {
        imagettftext($image, $size, 0.0, 15, 60, 0, $font, 'A');
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}

/* the type error already agreed with the signature and still does */
try {
    imageftbbox('x', 0.0, $font, 'A');
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
imageftbbox(): Argument #1 ($size) must be finite
imagettfbbox(): Argument #1 ($size) must be finite
imageftbbox(): Argument #1 ($size) must be between %i and %d
imagettfbbox(): Argument #1 ($size) must be between %i and %d
imageftbbox(): Argument #1 ($size) must be between %i and %d
imagettfbbox(): Argument #1 ($size) must be between %i and %d
imageftbbox(): Argument #1 ($size) must be between %i and %d
imagettfbbox(): Argument #1 ($size) must be between %i and %d
imagefttext(): Argument #2 ($size) must be finite
imagettftext(): Argument #2 ($size) must be finite
imagefttext(): Argument #2 ($size) must be between %i and %d
imagettftext(): Argument #2 ($size) must be between %i and %d
imageftbbox(): Argument #1 ($size) must be of type float, string given
