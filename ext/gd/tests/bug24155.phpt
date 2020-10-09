--TEST--
Bug #24155 (gdImageRotate270 rotation problem).
--SKIPIF--
<?php
    if (!extension_loaded('gd')) {
        die("skip gd extension not available\n");
    }
    if (!function_exists("imagerotate")) die("skip requires bundled GD library\n");
?>
--FILE--
<?php
    $dest = dirname(realpath(__FILE__)) . '/bug24155.png';
    @unlink($dest);

    $im = imagecreatetruecolor(30, 50);
    imagefill($im, 0, 0, (16777215 - 255));
    $im = imagerotate($im, 270, 255);
    imagepng($im, $dest);

    $im2 = imagecreatefrompng($dest);

    // Uniform fill + n x 90degrees rotation , the color value does not change
    $col = imagecolorat($im2, 20, 20);
    // 16777215 - 255 = 16776960
    echo "$col\n";

    @unlink($dest);
?>
--EXPECT--
16776960
