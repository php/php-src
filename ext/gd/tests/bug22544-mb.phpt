--TEST--
Bug #22544 (TrueColor transparency in PNG images).
--EXTENSIONS--
gd
--FILE--
<?php
    $image = imageCreateTruecolor(640, 100);
    $transparent = imageColorAllocate($image, 0, 0, 0);
    $red = imageColorAllocate($image, 255, 50, 50);
    imageColorTransparent($image, $transparent);
    imageFilledRectangle($image, 0, 0, 640-1, 100-1, $transparent);
    include_once __DIR__ . '/func.inc';
    test_image_equals_file(__DIR__ . '/bug22544私はガラスを食べられます.png', $image);
?>
--EXPECT--
The images are equal.
