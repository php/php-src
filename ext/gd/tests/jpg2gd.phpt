--TEST--
jpeg <--> gd1/gd2 conversion test
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!GD_BUNDLED && version_compare(GD_VERSION, '2.3.3', '>=')) {
        die("skip test requires GD 2.3.2 or older");
    }

    if (!function_exists("imagecreatefromjpeg") || !function_exists("imagejpeg")) {
        die("skip jpeg support unavailable");
    }
?>
--FILE--
<?php
    $cwd = __DIR__;

    echo "JPEG to GD1 conversion: ";
    echo imagegd(imagecreatefromjpeg($cwd . "/conv_test.jpg"), $cwd . "/test_jpeg.gd1") ? 'ok' : 'failed';
    echo "\n";

    echo "JPEG to GD2 conversion: ";
    echo imagegd2(imagecreatefromjpeg($cwd . "/conv_test.jpg"), $cwd . "/test_jpeg.gd2") ? 'ok' : 'failed';
    echo "\n";

    echo "GD1 to JPEG conversion: ";
    echo imagejpeg(imagecreatefromgd($cwd . "/test_jpeg.gd1"), $cwd . "/test_gd1.jpeg") ? 'ok' : 'failed';
    echo "\n";

    echo "GD2 to JPEG conversion: ";
    echo imagejpeg(imagecreatefromgd2($cwd . "/test_jpeg.gd2"), $cwd . "/test_gd2.jpeg") ? 'ok' : 'failed';
    echo "\n";

    @unlink($cwd . "/test_jpeg.gd1");
    @unlink($cwd . "/test_jpeg.gd2");
    @unlink($cwd . "/test_gd1.jpeg");
    @unlink($cwd . "/test_gd2.jpeg");
?>
--EXPECT--
JPEG to GD1 conversion: ok
JPEG to GD2 conversion: ok
GD1 to JPEG conversion: ok
GD2 to JPEG conversion: ok
