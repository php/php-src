--TEST--
Bug #38212 (Seg Fault on invalid imagecreatefromgd2part() parameters)
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!GD_BUNDLED && version_compare(GD_VERSION, '2.3.3', '>=')) {
        die("skip test requires GD 2.3.2 or older");
    }
?>
--FILE--
<?php
require __DIR__ . '/func.inc';

$file = __DIR__ . '/bug38212.gd2';
$im1 = imagecreatetruecolor(10,100);
imagefill($im1, 0,0, 0xffffff);
imagegd2($im1, $file);

trycatch_dump(
    fn() => imagecreatefromgd2part($file, 0,0, -25, 10),
    fn() => imagecreatefromgd2part($file, 0,0, 10, -25)
);

unlink($file);
?>
--EXPECT--
!! [ValueError] imagecreatefromgd2part(): Argument #4 ($width) must be greater than or equal to 1
!! [ValueError] imagecreatefromgd2part(): Argument #5 ($height) must be greater than or equal to 1
