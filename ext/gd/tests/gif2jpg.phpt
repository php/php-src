--TEST--
gif --> jpeg conversion test
--SKIPIF--
<?php
    if (!extension_loaded('gd')) {
        die("skip gd extension not available.");
    }

    if (!function_exists("imagejpeg")) {
        die("skip jpeg support unavailable");
    }
    if (!function_exists("imagecreatefromgif")) {
        die("skip gif read support unavailable");
    }
?>
--FILE--
<?php
    $cwd = __DIR__;

    echo "GIF to JPEG conversion: ";
    echo imagejpeg(imagecreatefromgif($cwd . "/conv_test.gif"), $cwd . "/test_gif.jpeg") ? 'ok' : 'failed';
    echo "\n";

    @unlink($cwd . "/test_gif.jpeg");
?>
--EXPECT--
GIF to JPEG conversion: ok
