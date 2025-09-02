--TEST--
png <--> gd1/gd2 conversion test
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!GD_BUNDLED && version_compare(GD_VERSION, '2.3.3', '>=')) {
        die("skip test requires GD 2.3.2 or older");
    }

    if (!function_exists("imagecreatefrompng") || !function_exists("imagepng")) {
        die("skip png support unavailable");
    }
?>
--FILE--
<?php
    $cwd = __DIR__;

    echo "PNG to GD1 conversion: ";
    echo imagegd(imagecreatefrompng($cwd . "/conv_test.png"), $cwd . "/test_png.gd1") ? 'ok' : 'failed';
    echo "\n";

    echo "PNG to GD2 conversion: ";
    echo imagegd2(imagecreatefrompng($cwd . "/conv_test.png"), $cwd . "/test_png.gd2") ? 'ok' : 'failed';
    echo "\n";

    echo "GD1 to PNG conversion: ";
    echo imagepng(imagecreatefromgd($cwd . "/test_png.gd1"), $cwd . "/test_gd1.png") ? 'ok' : 'failed';
    echo "\n";

    echo "GD2 to PNG conversion: ";
    echo imagepng(imagecreatefromgd2($cwd . "/test_png.gd2"), $cwd . "/test_gd2.png") ? 'ok' : 'failed';
    echo "\n";

    @unlink($cwd . "/test_png.gd1");
    @unlink($cwd . "/test_png.gd2");
    @unlink($cwd . "/test_gd1.png");
    @unlink($cwd . "/test_gd2.png");
?>
--EXPECT--
PNG to GD1 conversion: ok
PNG to GD2 conversion: ok
GD1 to PNG conversion: ok
GD2 to PNG conversion: ok
