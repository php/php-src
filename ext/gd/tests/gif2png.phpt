--TEST--
gif --> png conversion test
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!function_exists("imagepng")) {
        die("skip png support unavailable");
    }
    if (!function_exists("imagecreatefromgif")) {
        die("skip gif read support unavailable");
    }
?>
--FILE--
<?php
    $cwd = __DIR__;

    echo "GIF to PNG conversion: ";
    echo imagepng(imagecreatefromgif($cwd . "/conv_test.gif"), $cwd . "/test_gif.png") ? 'ok' : 'failed';
    echo "\n";

    @unlink($cwd . "/test_gif.png");
?>
--EXPECT--
GIF to PNG conversion: ok
