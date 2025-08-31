--TEST--
jpeg <--> png conversion test
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!function_exists("imagecreatefrompng") || !function_exists("imagepng")) {
        die("skip png support unavailable");
    }
    if (!function_exists("imagecreatefromjpeg") || !function_exists("imagejpeg")) {
        die("skip jpeg support unavailable");
    }
?>
--FILE--
<?php
    $cwd = __DIR__;

    echo "PNG to JPEG conversion: ";
    echo imagejpeg(imagecreatefrompng($cwd . "/conv_test.png"), $cwd . "/test_jpeg.jpeg") ? 'ok' : 'failed';
    echo "\n";

    echo "Generated JPEG to PNG conversion: ";
    echo imagepng(imagecreatefromjpeg($cwd . "/test_jpeg.jpeg"), $cwd . "/test_jpng.png") ? 'ok' : 'failed';
    echo "\n";

    echo "JPEG to PNG conversion: ";
    echo imagepng(imagecreatefromjpeg($cwd . "/conv_test.jpg"), $cwd . "/test_png.png") ? 'ok' : 'failed';
    echo "\n";

    echo "Generated PNG to JPEG conversion: ";
    echo imagejpeg(imagecreatefrompng($cwd . "/test_png.png"), $cwd . "/test_pjpeg.jpeg") ? 'ok' : 'failed';
    echo "\n";

    @unlink($cwd . "/test_jpeg.jpeg");
    @unlink($cwd . "/test_jpng.png");
    @unlink($cwd . "/test_png.png");
    @unlink($cwd . "/test_pjpeg.jpeg");
?>
--EXPECT--
PNG to JPEG conversion: ok
Generated JPEG to PNG conversion: ok
JPEG to PNG conversion: ok
Generated PNG to JPEG conversion: ok
