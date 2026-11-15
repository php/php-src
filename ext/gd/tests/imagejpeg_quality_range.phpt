--TEST--
imagejpeg(): the accepted range of $quality and the message for values outside it
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!function_exists('imagejpeg')) die('skip jpeg support unavailable');
?>
--FILE--
<?php
$image = imagecreatetruecolor(8, 8);
$file = __DIR__ . '/imagejpeg_quality_range.jpeg';

foreach ([-2, -1, 0, 100, 101] as $quality) {
    try {
        var_dump(imagejpeg($image, $file, $quality));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/imagejpeg_quality_range.jpeg');
?>
--EXPECT--
ValueError: imagejpeg(): Argument #3 ($quality) must be between -1 and 100
bool(true)
bool(true)
bool(true)
ValueError: imagejpeg(): Argument #3 ($quality) must be between -1 and 100
