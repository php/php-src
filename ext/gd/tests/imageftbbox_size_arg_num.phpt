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
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    try {
        imagettfbbox($size, 0.0, $font, 'A');
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

/* and argument #2 here */
foreach ([NAN, INF] as $size) {
    try {
        imagefttext($image, $size, 0.0, 15, 60, 0, $font, 'A');
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    try {
        imagettftext($image, $size, 0.0, 15, 60, 0, $font, 'A');
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

/* the type error already agreed with the signature and still does */
try {
    imageftbbox('x', 0.0, $font, 'A');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
ValueError: imageftbbox(): Argument #1 ($size) must be finite
ValueError: imagettfbbox(): Argument #1 ($size) must be finite
ValueError: imageftbbox(): Argument #1 ($size) must be between %i and %d
ValueError: imagettfbbox(): Argument #1 ($size) must be between %i and %d
ValueError: imageftbbox(): Argument #1 ($size) must be between %i and %d
ValueError: imagettfbbox(): Argument #1 ($size) must be between %i and %d
ValueError: imageftbbox(): Argument #1 ($size) must be between %i and %d
ValueError: imagettfbbox(): Argument #1 ($size) must be between %i and %d
ValueError: imagefttext(): Argument #2 ($size) must be finite
ValueError: imagettftext(): Argument #2 ($size) must be finite
ValueError: imagefttext(): Argument #2 ($size) must be between %i and %d
ValueError: imagettftext(): Argument #2 ($size) must be between %i and %d
TypeError: imageftbbox(): Argument #1 ($size) must be of type float, string given
