--TEST--
xpm --> jpeg conversion test
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!function_exists("imagejpeg")) {
        die("skip jpeg support unavailable");
    }
    if (!function_exists("imagecreatefromxpm")) {
        die("skip xpm read support unavailable");
    }
?>
--FILE--
<?php
    $cwd = __DIR__;

    echo "XPM to JPEG conversion: ";
    echo imagejpeg(imagecreatefromxpm($cwd . "/conv_test.xpm"), $cwd . "/test_xpm.jpeg") ? 'ok' : 'failed';
    echo "\n";

    @unlink($cwd . "/test_xpm.jpeg");
?>
--EXPECT--
XPM to JPEG conversion: ok
