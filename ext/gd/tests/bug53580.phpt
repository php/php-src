--TEST--
Bug #53580 (During resize gdImageCopyResampled cause colors change)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die("skip gd extension not available");
if (!GD_BUNDLED && version_compare(GD_VERSION, "2.3.2") <= 0) {
    die("skip test requires GD > 2.3.2");
}
?>
--FILE--
<?php
$w0 = 100;
$h0 = 100;
$w1 = 150;
$h1 = 150;
$c0 = 0xffffff;

$im0 = imagecreatetruecolor($w0, $h0);
imagefilledrectangle($im0, 0, 0, $w0 - 1, $h0 - 1, $c0);

$im1 = imagecreatetruecolor($w1, $h1);
imagecopyresampled($im1, $im0, 0, 0, 0, 0, $w1, $h1, $w0, $h0);

for ($i = 0; $i < $w1; $i++) {
    for ($j = 0; $j < $h1; $j++) {
        if (($c1 = imagecolorat($im1, $i, $j)) !== $c0) {
            printf("%d,%d = %d\n", $i, $j, $c1);
        }
    }
}
?>
--EXPECT--
