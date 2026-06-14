--TEST--
Bug #68629 (Transparent artifacts when using imagerotate)
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED && version_compare(GD_VERSION, "2.2.2") < 0) die("skip only fixed as of libgd 2.2.2");
?>
--FILE--
<?php
$im1 = imagecreatetruecolor(200, 200);
$trans = imagecolorallocatealpha($im1, 0, 0, 0, 127);
imagefill($im1, 0, 0, $trans);
$im2 = imagerotate($im1, 45, $trans);
$count = 0;
$width = imagesx($im2);
$height = imagesy($im2);
for ($y = 0; $y < $height; $y++) {
    for ($x = 0; $x < $width; $x++) {
        $rgb = imagecolorat($im2, $x, $y);
        if ($rgb !== 0x7f000000) {
            $count++;
        }
    }
}
var_dump($count);
?>
--EXPECT--
int(0)
