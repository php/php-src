--TEST--
Bug #73281 (imagescale(…, IMG_BILINEAR_FIXED) can cause black border)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.3', '<=')) die('skip upstream fix not yet released');
?>
--FILE--
<?php
$coordinates = [[0, 0], [0, 199], [199, 199], [199, 0]];

$src = imagecreatetruecolor(100, 100);
$white = imagecolorallocate($src, 255, 255, 255);
imagefilledrectangle($src, 0,0, 99,99, $white);
$dst = imagescale($src, 200, 200, IMG_BILINEAR_FIXED);
echo "truecolor source\n";
foreach ($coordinates as $coordinate) {
    list($x, $y) = $coordinate;
    printf("%3d, %3d: %x\n", $x, $y, imagecolorat($dst, $x, $y));
}

$src = imagecreate(100, 100);
$white = imagecolorallocate($src, 255, 255, 255);
imagefilledrectangle($src, 0,0, 99,99, $white);
$dst = imagescale($src, 200, 200, IMG_BILINEAR_FIXED);
echo "\npalette source\n";
foreach ($coordinates as $coordinate) {
    list($x, $y) = $coordinate;
    printf("%3d, %3d: %x\n", $x, $y, imagecolorat($dst, $x, $y));
}
?>
===DONE===
--EXPECT--
truecolor source
  0,   0: ffffff
  0, 199: ffffff
199, 199: ffffff
199,   0: ffffff

palette source
  0,   0: ffffff
  0, 199: ffffff
199, 199: ffffff
199,   0: ffffff
===DONE===
