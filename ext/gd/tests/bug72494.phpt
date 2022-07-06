--TEST--
Bug #72494 (imagecropauto out-of-bounds access)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
require __DIR__ . '/func.inc';

$im = imagecreate(10,10);

trycatch_dump(
    fn() => imagecropauto($im, IMG_CROP_THRESHOLD, 0, 1337)
);

?>
--EXPECT--
!! [ValueError] imagecropauto(): Argument #4 ($color) must be greater than or equal to 0 when using the threshold mode
