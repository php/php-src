--TEST--
Bug #72494 (imagecropauto out-of-bounds access)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
require __DIR__ . '/test_helpers.inc';

$im = imagecreate(10,10);

trycatch_dump(
    fn() => imagecropauto($im, IMG_CROP_THRESHOLD, 0, 1337)
);

?>
--EXPECT--
!! [Error] Color argument missing with threshold mode
