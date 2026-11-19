--TEST--
Bug #72494 (imagecropauto out-of-bounds access)
--EXTENSIONS--
gd
--FILE--
<?php
require __DIR__ . '/func.inc';

$im = imagecreate(10,10);

trycatch_dump(
    fn() => imagecropauto($im, IMG_CROP_THRESHOLD, 0, 1337)
);

?>
--EXPECT--
!! [ValueError] imagecropauto(): Argument #4 ($color) must be a valid color index of the image when using the threshold mode
