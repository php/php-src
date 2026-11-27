--TEST--
imagesetbrush(), imagesettile(): the brush and tile images stay alive while they are in use
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreatetruecolor(20, 20);
$brush = imagecreatetruecolor(4, 4);
imagefill($brush, 0, 0, 0xff0000);
imagesetbrush($im, $brush);
unset($brush);

$junk = [];
for ($i = 0; $i < 50; $i++) {
    $j = imagecreatetruecolor(4, 4);
    imagefill($j, 0, 0, 0x00ff00);
    $junk[] = $j;
}
imageline($im, 2, 10, 17, 10, IMG_COLOR_BRUSHED);
printf("%06x\n", imagecolorat($im, 5, 10));

$tile = imagecreatetruecolor(2, 2);
imagefill($tile, 0, 0, 0x0000ff);
imagesettile($im, $tile);
unset($tile);
$junk = [];
for ($i = 0; $i < 50; $i++) {
    $j = imagecreatetruecolor(2, 2);
    imagefill($j, 0, 0, 0x00ff00);
    $junk[] = $j;
}
imagefilledrectangle($im, 0, 0, 19, 19, IMG_COLOR_TILED);
printf("%06x\n", imagecolorat($im, 5, 5));

// A new brush replaces the old one, and a cycle is collected
$a = imagecreatetruecolor(4, 4);
imagesetbrush($a, $a);
imagesettile($a, $a);
$w = WeakReference::create($a);
unset($a);
gc_collect_cycles();
var_dump($w->get());
?>
--EXPECT--
ff0000
0000ff
NULL
